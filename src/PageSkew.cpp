#include "PageSkew.h"
using namespace cv;
using namespace std;
/*
////--------------------------------------------------------------
//void PageSkew::setup() {
//
//	ostringstream classSize;
//
//	classSize << 4;
//	_putenv_s("OMP_THREAD_LIMIT", classSize.str().c_str());
//
//	setlocale(LC_ALL, "rus");
//
//	image.load("page.png");
//	pixelImage = image.getPixels();
//	scaleImage = 1;
//	image = getSkew(pixelImage);
//	image.saveImage("result1.jpg");
//}
/*
cv::Mat PageSkew::getCvMatFromOfImage(ofImage& img) {
	auto cvImg = cv::Mat(img.getHeight(), img.getWidth(), CV_MAKETYPE(CV_8U, img.getPixels().getNumChannels()), img.getPixels().getData());
	return cvImg.clone();
}

cv::Mat PageSkew::getCvMatFromOfPixels(ofPixels& img) {
	auto cvImg = cv::Mat(img.getHeight(), img.getWidth(), CV_MAKETYPE(CV_8U, img.getNumChannels()), img.getData());
	return cvImg.clone();
}

ofImage PageSkew::getOfImageFromCvImage(cv::Mat& mat) {
	ofImage img;
	// Если 3 канала:
	if (mat.channels() == 3) img.setFromPixels(mat.data, mat.cols, mat.rows, OF_IMAGE_COLOR);
	if (mat.channels() == 4) img.setFromPixels(mat.data, mat.cols, mat.rows, OF_IMAGE_COLOR_ALPHA);
	if (mat.channels() == 1) img.setFromPixels(mat.data, mat.cols, mat.rows, OF_IMAGE_GRAYSCALE);

	return img;
}

ofPixels PageSkew::getSkew(ofPixels& pixels)
{
	cv::Mat img = getCvMatFromOfPixels(pixels);
	std::vector<cv::Mat>   channels;
	cv::split(img, channels);

	double angle = computeSkew(pixels, channels[0]);
	cv::Point2f pc(pixels.getWidth() / 2., pixels.getHeight() / 2.);
	cv::Mat r = cv::getRotationMatrix2D(pc, angle, 1.0);

	cv::Mat dst;

	cv::warpAffine(img, dst, r, img.size(), cv::INTER_CUBIC);

	//fillBorder(dst);

	image = getOfImageFromCvImage(dst);
	pixels = image.getPixels();
	image.saveImage("resultRotated.jpg");
	return pixels;
}

void PageSkew::fillBorder(cv::Mat& dst)
{
	Rect rect;
	int valuePixel = dst.at<uchar>(0, 0);
	int valuePixel1 = dst.at<uchar>(dst.cols - 1, 0);
	int valuePixel2 = dst.at<uchar>(dst.cols - 1, dst.rows - 1);
	int valuePixel3 = dst.at<uchar>(0, dst.rows - 1);

	if (valuePixel == 0)
		cv::floodFill(dst, Point(0, 0), Scalar(255, 255, 255), &rect);

	if (valuePixel1 == 0)
		cv::floodFill(dst, Point(dst.cols - 1, 0), Scalar(255, 255, 255), &rect);

	if (valuePixel2 == 0)
		cv::floodFill(dst, Point(dst.cols - 1, dst.rows - 1), Scalar(255, 255, 255), &rect);

	if (valuePixel3 == 0)
		cv::floodFill(dst, Point(0, dst.rows - 1), Scalar(255, 255, 255), &rect);
}

double   PageSkew::computeSkew(ofPixels& pixels, cv::Mat img)
{
	// Load in grayscale.	

	int startCalcTime = ofGetElapsedTimeMillis();
	image = getOfImageFromCvImage(img);
	image.save("skew_0_chanel0.png");

	// Binarize
	cv::threshold(img, img, 225, 255, cv::THRESH_BINARY);

	image = getOfImageFromCvImage(img);
	image.save("skew_0_binarize.png");

	//раздуваем размер 
	cv::Mat horizontStructure = getStructuringElement(cv::MORPH_RECT, cvSize(3, 1));
	cv::erode(img, img, horizontStructure);
	cv::erode(img, img, horizontStructure);


	//cv::dilate(img, img, horizontStructure);

	image = getOfImageFromCvImage(img);
	image.save("skew_0_dilate.png");

	// Invert colors
	cv::bitwise_not(img, img);
	image = getOfImageFromCvImage(img);
	image.save("skew_1_invert.png");

	ofxCvGrayscaleImage grayImg;
	grayImg.setFromPixels(image.getPixelsRef());

	ofxCvContourFinder contourFinder;
	contourFinder.findContours(grayImg, 500, 10000, 1000, false);
	
	

	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 3));
	cv::erode(img, img, element);
	image = getOfImageFromCvImage(img);
	image.save("skew_2_erode.png");

	std::vector<cv::Point> points;

	cv::Mat_<uchar> ::iterator it = img.begin<uchar>();
	cv::Mat_<uchar> ::iterator end = img.end<uchar>();

	for (; it != end; ++it)
		if (*it)
			points.push_back(it.pos());

	cv::Point2f vertices[4];
	cv::RotatedRect box = cv::minAreaRect(points);
	double angle = box.angle;
	if (angle < -45.)
		angle += 90.;

	box.points(vertices);

	for (int i = 0; i < 4; ++i)
	{
		cout << "vertices " << vertices[i] << endl;
		cv::line(img, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 0, 0), 1, CV_AA);
	}
	cout << "angle " << angle << endl;

	image = getOfImageFromCvImage(img);
	cout << "calc time: " << ofGetElapsedTimeMillis() - startCalcTime << endl;

	image.saveImage("skew_3_result.png");
	return angle;
}
*/