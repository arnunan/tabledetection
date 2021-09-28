#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include <opencv2/opencv.hpp>

#define  PAGE_SKEW_BLOB_THRESHOLD 500

class Utils
{
public:
	static	cv::Mat getCvMatFromOfPixels(ofPixels& img);
	static	ofImage getOfImageFromCvImage(cv::Mat& mat);
};

struct Blob {
	Blob(vector<cv::Point> contour);
	~Blob();

	void draw(int x, int y);

	static bool sortByAngle(Blob & a, Blob & b);


	vector<cv::Point>	contour;
	cv::RotatedRect		rotatedRect;
	float				area;
	float				angle;


private:
	ofColor		color;
	cv::Point2f vertices[4];
};

class PageSkew{
public:
	PageSkew() {};
	~PageSkew() {};

	void draw(int x = 0, int y = 0);
	void skew(ofPixelsRef & inpustPixRef);

	vector<ofImage>	images;
	vector<Blob>	blobs;				//первоначальный набор блобов
	vector<Blob>	blobsSizeFiltered;	//фильтрованные по размеру 
};