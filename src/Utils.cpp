#include "Utils.h"

cv::Mat Utils::getCvMatFromOfPixels(ofPixels& img) {
	auto cvImg = cv::Mat(img.getHeight(), img.getWidth(), CV_MAKETYPE(CV_8U, img.getNumChannels()), img.getData());
	return cvImg.clone();
}

ofImage Utils::getOfImageFromCvImage(cv::Mat& mat) {
	ofImage img;
	// ≈сли 3 канала:
	if (mat.channels() == 3) img.setFromPixels(mat.data, mat.cols, mat.rows, OF_IMAGE_COLOR);
	if (mat.channels() == 4) img.setFromPixels(mat.data, mat.cols, mat.rows, OF_IMAGE_COLOR_ALPHA);
	if (mat.channels() == 1) img.setFromPixels(mat.data, mat.cols, mat.rows, OF_IMAGE_GRAYSCALE);

	return img;
}

Blob::Blob(vector<cv::Point> contour) {
	this->contour = contour;
	rotatedRect = cv::minAreaRect(contour);
	area = cv::contourArea(contour);

	color = ofColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255));
	rotatedRect.points(vertices);

	angle = rotatedRect.angle;
	if (angle < -45) angle += 90;
}

Blob::~Blob() {}

void Blob::draw(int x, int y) {
	ofPushMatrix();
	ofPushStyle();
	ofTranslate(x, y);

	//контур
	ofSetColor(color);
	ofPolyline pll;
	for (size_t j = 0; j < contour.size(); j++) {
		pll.addVertex(contour[j].x, contour[j].y, 0);
	}
	pll.close();
	pll.draw();

	//пр€моугольник
	ofSetLineWidth(2);
	pll.clear();
	pll.addVertex(vertices[0].x, vertices[0].y);
	pll.addVertex(vertices[1].x, vertices[1].y);
	pll.addVertex(vertices[2].x, vertices[2].y);
	pll.addVertex(vertices[3].x, vertices[3].y);
	pll.close();
	pll.draw();

	ofCircle(pll[0], 5);

	ofPopStyle();
	ofPopMatrix();
}


bool Blob::sortByAngle(Blob & a, Blob & b) {
	return a.angle > b.angle;
}



void PageSkew::skew(ofPixelsRef & inpustPixRef) {
	images.clear();
	images.push_back(ofImage(inpustPixRef));
	blobs.clear();
	blobsSizeFiltered.clear();

	//раздел€ем каналы 
	cv::Mat img3ch = Utils::getCvMatFromOfPixels(images.back().getPixelsRef());
	std::vector<cv::Mat>   channels;
	cv::split(img3ch, channels);

	cv::Mat img0ch = channels[0];

	// Binarize
	cv::threshold(img0ch, img0ch, 225, 255, cv::THRESH_BINARY);
	images.push_back(Utils::getOfImageFromCvImage(img0ch));

	// Invert colors
	cv::bitwise_not(img0ch, img0ch);
	images.push_back(Utils::getOfImageFromCvImage(img0ch));


	//раздуваем размер 
	cv::Mat horizontStructure = getStructuringElement(cv::MORPH_RECT, cvSize(3, 3));
	cv::dilate(img0ch, img0ch, horizontStructure);
	cv::dilate(img0ch, img0ch, horizontStructure);
	images.push_back(Utils::getOfImageFromCvImage(img0ch));

	//находим контуры 
	vector<cv::Vec4i>			hierarchy;
	vector<vector<cv::Point>>	contours;
	cv::findContours(img0ch, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
	for (size_t i = 0; i < contours.size(); i++)
		blobs.push_back(Blob(contours[i]));

	//фильтруем по площади 
	for (size_t i = 0; i < blobs.size(); i++)
	{
		if (blobs[i].area > PAGE_SKEW_BLOB_THRESHOLD) blobsSizeFiltered.push_back(blobs[i]);
	}
	//сортируем по углу 
	std::sort(blobsSizeFiltered.begin(), blobsSizeFiltered.end(), Blob::sortByAngle);

	float resultAngle = 0;
	resultAngle = blobsSizeFiltered[blobsSizeFiltered.size() / 2].angle;

	cv::Point2f pc(images[0].getWidth() / 2., images[0].getHeight() / 2.);
	cv::Mat r = cv::getRotationMatrix2D(pc, resultAngle, 1.0);
	cv::Mat dst;
	cv::warpAffine(img3ch, dst, r, img3ch.size(), cv::INTER_CUBIC);
	images.push_back(Utils::getOfImageFromCvImage(dst));
};

void PageSkew::draw(int x, int y) {
	if (!images.size())return;
	ofPushMatrix();
	ofTranslate(x, y);

	for (size_t i = 0; i < images.size() - 1; i++)
		images[i].draw(images[i].getWidth()*i, 0);

	//не фильтрованные блобы
	images[0].draw(0, images[0].getHeight());
	for (size_t i = 0; i < blobs.size(); i++)
		blobs[i].draw(0, images[0].getHeight());

	//фильтрованные блобы
	images[0].draw(images[0].getWidth(), images[0].getHeight());
	for (size_t i = 0; i < blobsSizeFiltered.size(); i++)
		blobsSizeFiltered[i].draw(images[0].getWidth(), images[0].getHeight());

	ofSetColor(255, 255, 255);
	//финальное изображение 
	images.back().draw(images[0].getWidth() * 3, images[0].getHeight());

	ofPopMatrix();
	ofSetColor(255, 255, 255);
}