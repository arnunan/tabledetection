#include "TablePageManager.h"

TablePageManager::TablePageManager() {
};
TablePageManager::~TablePageManager() {
	deleteTables();
};

void TablePageManager::draw(int x, int y, bool drawBlobs) {

	ofPushMatrix();

	ofTranslate(x, y);

	for (size_t i = 0; i < images.size(); i++)
		images[i].draw(images[i].getWidth() * i, 0);
	
	ofTranslate(0, images[0].getHeight());

	if (drawBlobs) {
		for (size_t i = 0; i < blobs.size(); i++)
			blobs[i].draw(0, 0);		
	}
	else {
		for (size_t i = 0; i < tables.size(); i++)
			tables[i]->draw();		
	}

	images[0].draw(images[0].getWidth(), 0);
	ofPopMatrix();
}


void TablePageManager::recognizePage(string path) {
	ofImage image;
	image.load(path);
	recognize(image.getPixelsRef());
}

void TablePageManager::recognizePage(ofPixels &inputPixels) {
	recognize(inputPixels);
}

void TablePageManager::recognize(ofPixels &pix) {
	deleteTables();
	images.clear();

	cv::Mat mat = getCvMatFromOfPixels(pix);

	//подготавливаем изображение для поиска блобов
	ofxCvGrayscaleImage grayImg = prepareGrayImage(mat);

	//находим прямоугольники блобов
	vector<Cell*>	allCells = findBlodRectsFromImage(grayImg);
	//находим джоинты 
	vector<Joint>	joints = findJoints(allCells);
	//объеденяем джоинты
	vector<Joint*>	allJoints = mergeJoints(joints);

	//разделяем ячейки и джоинты для ассоциации с разными таблицами
	vector<vector<Cell*>> splitedCells;
	vector<vector<Joint*>> splitedJoints;
	splitTables(allCells, allJoints, splitedCells, splitedJoints);

	//фильтруем таблицы по количеству ячеек 
	filterSmallTables(splitedCells, splitedJoints);

	//создаем таблицы из массивов
	for (size_t i = 0; i < splitedCells.size(); i++)
		tables.push_back(new Table(splitedCells[i], splitedJoints[i]));
}

ofxCvGrayscaleImage TablePageManager::prepareGrayImage(cv::Mat mat)
{
	cv::Mat src_gray, dst, fin;

	cvtColor(mat, src_gray, cv::COLOR_BGR2GRAY);	// Convert the image to Gray
	ofImage		tmp;

	cv::Mat bw;
	cv::adaptiveThreshold(~src_gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 15, -2);

	tmp = getOfImageFromCvImage(bw);
	tmp.save("1_1.png");
	images.push_back(tmp);

	// Create the images that will use to extract the horizonta and vertical lines
	cv::Mat horizontal = bw.clone();
	cv::Mat vertical = bw.clone();

	int scale = 15; // play with this variable in order to increase/decrease the amount of lines to be detected

	// Specify size on horizontal axis
	int horizontalsize = horizontal.cols / TABEL_HORIZONTAL_MORFOLOGY_DIVIDER;

	// Create structure element for extracting horizontal lines through morphology operations
	cv::Mat horizontalStructure = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(horizontalsize, 1));

	// Apply morphology operations
	erode(horizontal, horizontal, horizontalStructure, cv::Point(-1, -1));
	dilate(horizontal, horizontal, horizontalStructure, cv::Point(-1, -1));
	// dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1)); // expand horizontal lines

	tmp = getOfImageFromCvImage(horizontal);
	tmp.save("2_hor_kernel.png");
	images.push_back(tmp);

	// Specify size on vertical axis
	int verticalsize = vertical.rows / TABEL_VERTICAL_MORFOLOGY_DIVIDER;

	// Create structure element for extracting vertical lines through morphology operations
	cv::Mat verticalStructure = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, verticalsize));

	// Apply morphology operations
	erode(vertical, vertical, verticalStructure, cv::Point(-1, -1));
	dilate(vertical, vertical, verticalStructure, cv::Point(-1, -1));
	//    dilate(vertical, vertical, verticalStructure, Point(-1, -1)); // expand vertical lines
	tmp = getOfImageFromCvImage(vertical);
	tmp.save("3_ver_kernel.png");
	images.push_back(tmp);

	cv::Mat mask = horizontal + vertical;
	cv::dilate(mask, mask, getStructuringElement(cv::MORPH_CROSS, cvSize(3, 3)));
	tmp = getOfImageFromCvImage(mask);
	tmp.save("4_fin.png");
	images.push_back(tmp);

	ofxCvGrayscaleImage grayImg;
	tmp.setImageType(OF_IMAGE_GRAYSCALE);
	grayImg.setFromPixels(tmp.getPixels());

	return grayImg;
}
vector<Cell*>		TablePageManager::findBlodRectsFromImage(ofxCvGrayscaleImage& inputImage) {
	vector<Cell*> rects;

	blobs.clear();
	/// Выполняем поиск контуров на изображении:	
	vector<cv::Vec4i>			hierarchy;
	vector<vector<cv::Point>>	contours;
	cv::Mat input = getCvMatFromOfPixels(inputImage.getPixels());
	cv::findContours(input, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
	for (size_t i = 0; i < contours.size(); i++) {
		Blob b = Blob(contours[i]);
		if (b.rotatedRect.boundingRect().height > JOINT_UNION_DISTANCE * 2 && b.rotatedRect.boundingRect().width > JOINT_UNION_DISTANCE * 2) blobs.push_back(b);
	}

	cout << "before filter big: " << blobs.size() << endl;
	vector<Blob> toRemove;
	for (size_t i = 0; i < blobs.size(); i++) {
		cv::Rect bound = blobs[i].rotatedRect.boundingRect();
		ofRectangle bRect(bound.x, bound.y, bound.width, bound.height);
		int includes = 0;
		for (size_t j = 0; j < blobs.size(); j++) {
			if (i != j) {
				cv::Rect bound2 = blobs[j].rotatedRect.boundingRect();
				ofRectangle bRect2(bound2.x, bound2.y, bound2.width, bound2.height);
				if (bRect.inside(bRect2)) includes++;
			}
		}
		if (includes > 0) {
			cout << "num inc: " << includes << endl;
			toRemove.push_back(blobs[i]);
			blobs.erase(blobs.begin() + i);
			i--;
		}
	}

	cout << " toRemove: " << toRemove.size() << endl;
	cout << " after filter big blobs: " << blobs.size() << endl;

	for (size_t i = 0; i < blobs.size(); i++) {
		cv::Rect bound = blobs[i].rotatedRect.boundingRect();
		if (bound.height > JOINT_UNION_DISTANCE * 2 &&
			bound.width > JOINT_UNION_DISTANCE * 2) {
			rects.push_back(new Cell());
			rects.back()->rect.set(bound.x, bound.y, bound.width, bound.height);
		}
	}

	cout << "rects: " << rects.size() << endl;
	return rects;
}

cv::Mat TablePageManager::getCvMatFromOfImage(ofImage& img) {
	auto cvImg = cv::Mat(img.getHeight(), img.getWidth(), CV_MAKETYPE(CV_8U, img.getPixels().getNumChannels()), img.getPixels().getData());
	return cvImg.clone();
}

cv::Mat TablePageManager::getCvMatFromOfPixels(ofPixelsRef inputPixels) {
	auto cvImg = cv::Mat(inputPixels.getHeight(), inputPixels.getWidth(), CV_MAKETYPE(CV_8U, inputPixels.getNumChannels()), inputPixels.getData());
	return cvImg.clone();
}

ofImage TablePageManager::getOfImageFromCvImage(cv::Mat& mat) {
	ofImage img;
	// Если 3 канала:
	if (mat.channels() == 3) img.setFromPixels(mat.data, mat.cols, mat.rows, OF_IMAGE_COLOR);
	if (mat.channels() == 4) img.setFromPixels(mat.data, mat.cols, mat.rows, OF_IMAGE_COLOR_ALPHA);
	if (mat.channels() == 1) img.setFromPixels(mat.data, mat.cols, mat.rows, OF_IMAGE_GRAYSCALE);
	return img;
}

void TablePageManager::threshold(cv::Mat& gray, cv::Mat& _dst)
{
	int threshold_value = 128;
	int threshold_type = 1;
	int const max_value = 255;
	int const max_type = 4;
	int const max_binary_value = 255;
	cv::threshold(gray, _dst, threshold_value, max_binary_value, threshold_type);
}

vector<Joint> TablePageManager::findJoints(vector<Cell*> &cells) {
	vector<Joint> joints;

	//находим все возможные джоинты 
	for (size_t i = 0; i < cells.size(); i++) {
		ofRectangle & rect = cells[i]->rect;

		joints.push_back(Joint(rect.x, rect.y));
		joints.back().leftUpForThisCell = cells[i];

		joints.push_back(Joint(rect.x, rect.y + rect.height));
		joints.back().leftDownForThisCell = cells[i];

		joints.push_back(Joint(rect.x + rect.width, rect.y));
		joints.back().rightUpForThisCell = cells[i];

		joints.push_back(Joint(rect.x + rect.width, rect.y + rect.height));
		joints.back().rightDownForThisCell = cells[i];
	}

	return joints;
}

vector<Joint*>	TablePageManager::mergeJoints(vector<Joint> joints) {

	vector<Joint*> resultJoints;

	//объеденяем джоинты с одинаковыми координатами и формируем конечный список джоинтов
	while (joints.size())
	{
		resultJoints.push_back(new Joint(joints.back()));
		joints.pop_back();
		//даем ссылку на джоинт в ячейки
		Joint * joint = resultJoints.back();

		if (joint->leftDownForThisCell)		joint->leftDownForThisCell->leftDown = joint;
		if (joint->leftUpForThisCell)		joint->leftUpForThisCell->leftUp = joint;
		if (joint->rightDownForThisCell)	joint->rightDownForThisCell->rightDown = joint;
		if (joint->rightUpForThisCell)		joint->rightUpForThisCell->rightUp = joint;

		for (int i = joints.size() - 1; i >= 0; i--)
		{
			//если токи достаточно близко то объеденяем
			if ((joint->pos - joints[i].pos).length() < JOINT_UNION_DISTANCE) {
				Cell * cell;
				//записываем клетку к которой принадлежит данный джоинт
				//leftDown
				if (joints[i].leftDownForThisCell) {
					cell = joints[i].leftDownForThisCell;
					//даем клетке ссылку на джоинт
					cell->leftDown = resultJoints.back();
					//даем джоинту ссылку на клетку
					joint->leftDownForThisCell = cell;
				}

				//записываем клетку к которой принадлежит данный джоинт
				//leftUp
				if (joints[i].leftUpForThisCell) {
					cell = joints[i].leftUpForThisCell;
					//даем клетке ссылку на джоинт
					cell->leftUp = resultJoints.back();
					//даем джоинту ссылку на клетку
					joint->leftUpForThisCell = cell;
				}

				//записываем клетку к которой принадлежит данный джоинт
				//rightDown
				if (joints[i].rightDownForThisCell) {
					cell = joints[i].rightDownForThisCell;
					//даем клетке ссылку на джоинт
					cell->rightDown = resultJoints.back();
					//даем джоинту ссылку на клетку
					joint->rightDownForThisCell = cell;
				}

				//записываем клетку к которой принадлежит данный джоинт
				//rightUp
				if (joints[i].rightUpForThisCell) {
					cell = joints[i].rightUpForThisCell;
					//даем клетке ссылку на джоинт
					cell->rightUp = resultJoints.back();
					//даем джоинту ссылку на клетку
					joint->rightUpForThisCell = cell;
				}

				joints.erase(joints.begin() + i);
			}
		}
	}
	return resultJoints;
}

void TablePageManager::filterSelfConections(vector<Cell*> &cells) {
	for (size_t i = 0; i < cells.size(); i++) {
		Cell * cell = cells[i];
		if (cell->leftUp == cell->leftDown || cell->leftUp == cell->rightDown || cell->leftUp == cell->rightUp) cell->leftUp = NULL;
		if (cell->leftDown == cell->leftUp || cell->leftDown == cell->rightDown || cell->leftDown == cell->rightUp) cell->leftDown = NULL;
		if (cell->rightUp == cell->leftDown || cell->rightUp == cell->rightDown || cell->rightUp == cell->leftUp)  cell->rightUp = NULL;
		if (cell->rightDown == cell->leftDown || cell->rightDown == cell->rightDown || cell->leftUp == cell->rightUp) cell->leftDown = NULL;
	}
}

void TablePageManager::splitTables(vector<Cell*> cells, vector<Joint*> joints, vector<vector<Cell*>> &splitedCells, vector<vector<Joint*>> &splitedJoints) {
	//чистим массивы
	splitedCells.clear();
	splitedJoints.clear();

	splitedJoints.push_back(vector<Joint*>());
	splitedCells.push_back(vector<Cell*>());

	while (cells.size() > 0) {
		//временный ветор для ечеек связи от которых мы не просчитали 
		vector<Cell*> cellsTmp;

		//связи для этой клетки нужно обработать
		cellsTmp.push_back(cells.back());

		//эта клетка сохраненная как первая для новой таблицы 
		splitedCells.back().push_back(cells.back());

		//подтираем т.к. добавили её в splitedCells
		cells.pop_back();

		for (size_t i = 0; i < cellsTmp.size(); i++)
		{
			Cell* cell = cellsTmp[i];
			//все джоинты данной клетки 
			vector<Joint*> conectedJoints;
			if (cell->leftDown)
				conectedJoints.push_back(cell->leftDown);
			if (cell->leftUp)
				conectedJoints.push_back(cell->leftUp);
			if (cell->rightDown)
				conectedJoints.push_back(cell->rightDown);
			if (cell->rightUp)
				conectedJoints.push_back(cell->rightUp);

			// ассоциируем джжоинты с таблицей
			for (size_t j = 0; j < conectedJoints.size(); j++)
			{
				for (size_t k = 0; k < joints.size(); k++)
				{
					if (conectedJoints[j] == joints[k]) {
						splitedJoints.back().push_back(joints[k]);
						joints.erase(joints.begin() + k);
						break;
					}
				}
			}

			//клетки имеющие общий джоинт с данной (соседи)
			vector<Cell*> neightboards;
			for (size_t j = 0; j < conectedJoints.size(); j++)
			{
				Joint* joint = conectedJoints[j];
				if (joint->leftUpForThisCell && cell != joint->leftUpForThisCell)
					neightboards.push_back(joint->leftUpForThisCell);
				if (joint->leftDownForThisCell && cell != joint->leftDownForThisCell)
					neightboards.push_back(joint->leftDownForThisCell);
				if (joint->rightUpForThisCell && cell != joint->rightUpForThisCell)
					neightboards.push_back(joint->rightUpForThisCell);
				if (joint->rightDownForThisCell && cell != joint->rightDownForThisCell)
					neightboards.push_back(joint->rightDownForThisCell);
			}

			for (size_t j = 0; j < neightboards.size(); j++) {
				//если есть в массиве неразмеченных то удаляем от туда и добавляем в список для конкретной таблицы а также в список для проверки её соседей 
				for (size_t k = 0; k < cells.size(); k++) {
					if (neightboards[j] == cells[k]) {
						splitedCells.back().push_back(cells[k]);
						cellsTmp.push_back(cells[k]);
						cells.erase(cells.begin() + k);
						break;
					}
				}
			}
		}

		//если мы обработали все связи из cellsTmp и в начальном массиве остались
		//эллементы то ужно их забить в новую таблицу 
		if (cells.size() > 0) {
			splitedCells.push_back(vector<Cell*>());
			splitedJoints.push_back(vector<Joint*>());
		}
	}
}

void TablePageManager::filterSmallTables(vector<vector<Cell*>> &splitedCells, vector<vector<Joint*>> &splitedJoints) {
	for (int i = splitedCells.size() - 1; i >= 0; i--) {
		if (splitedCells[i].size() <= TABEL_NUM_CELLS_THRESHOLD) {

			for (size_t j = 0; j < splitedCells[i].size(); j++)
				delete splitedCells[i][j];
			for (size_t j = 0; j < splitedJoints[i].size(); j++)
				delete splitedJoints[i][j];

			splitedCells.erase(splitedCells.begin() + i);
			splitedJoints.erase(splitedJoints.begin() + i);
		}
	}
}

void TablePageManager::deleteTables() {
	for (size_t i = 0; i < tables.size(); i++)
		delete tables[i];
	tables.clear();
}