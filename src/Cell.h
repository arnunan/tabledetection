#pragma once
#include "ofMain.h"
#include "Joint.h"

class Cell {
public:
	Cell();
	Cell(ofRectangle rect);
	void initJointsPtr();
	void draw(ofColor clr);

	ofRectangle rect;

	vector<int> x;
	vector<int> y;
	vector<int> row;
	vector<int> col;

	Joint * leftUp;
	Joint * leftDown;
	Joint * rightUp;
	Joint * rightDown;
};
