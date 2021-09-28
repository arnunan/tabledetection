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

	ofVec2f pos; //������� �� �����������

	//���������� � �������
	int x;
	int y;

	//������ ������ �������� "leftUp" ��� ������ leftUp
	Cell * leftUpForThisCell;
	Cell * leftDownForThisCell;
	Cell * rightUpForThisCell;
	Cell * rightDownForThisCell;

private:
	void initCellPtr();
};
