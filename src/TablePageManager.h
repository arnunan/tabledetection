#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"
#include "ofImage.h"
#include "Table.h"
#include "PageSkew.h"
#include <opencv2/opencv.hpp>
#include "Utils.h"

#define TABEL_NUM_CELLS_THRESHOLD	2
#define TABEL_HORIZONTAL_MORFOLOGY_DIVIDER	15
#define TABEL_VERTICAL_MORFOLOGY_DIVIDER	70

class TablePageManager
{
public:
	TablePageManager();
	~TablePageManager();

	void draw(int x, int y, bool drawBlobs);
	void recognizePage(string path);
	void recognizePage(ofPixels &inputPixels);
	
private:
	ofxCvGrayscaleImage prepareGrayImage(cv::Mat mat);	
	vector<Cell*>       findBlodRectsFromImage(ofxCvGrayscaleImage& inputImage);

	ofImage getOfImageFromCvImage(cv::Mat & mat);
	cv::Mat getCvMatFromOfImage(ofImage & img);
	cv::Mat getCvMatFromOfPixels(ofPixelsRef inputPixels);

	vector<Joint>	findJoints(vector<Cell*> &cells);
	vector<Joint*>	mergeJoints(vector<Joint> joints);
	void			filterSelfConections(vector<Cell*> &cells);
	void			splitTables(vector<Cell*> cells, vector<Joint*> joints, vector<vector<Cell*>> &splitedCells, vector<vector<Joint*>> &splitedJoints);
	void			filterSmallTables(vector<vector<Cell*>> &splitedCells, vector<vector<Joint*>> &splitedJoints);
	void            recognize(ofPixels &pix);
	void            deleteTables();
	void            threshold(cv::Mat& gray, cv::Mat& _dst);

	vector<Table*>	tables;
	vector<uint>    histiogram;
	vector<ofImage>	images;
	vector<Blob>    blobs;

};
