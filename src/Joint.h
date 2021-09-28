#pragma once
#include "ofMain.h"

#define JOINT_UNION_DISTANCE		5

class Cell;

class Joint {
public:

	Joint();
	Joint(ofVec2f pos);
	Joint(float x, float y);
	~Joint();

	void draw();

	ofVec2f pos; //позици€ на изображении

	//координаты в таблице
	int x;
	int y;

	//данный джоинт €вл€етс€ "leftUp" дл€ €чейки leftUp
	Cell * leftUpForThisCell;
	Cell * leftDownForThisCell;
	Cell * rightUpForThisCell;
	Cell * rightDownForThisCell;

private:
	void initCellPtr();
};
