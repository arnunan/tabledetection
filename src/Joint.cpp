#include "Joint.h"

Joint::Joint(ofVec2f pos) {
	this->pos = pos;
	initCellPtr();
}

Joint::Joint(float x, float y) {
	this->pos.x = x;
	this->pos.y = y;
	initCellPtr();
}

Joint::Joint() {
	initCellPtr();
}

Joint::~Joint() {
}

void Joint::initCellPtr() {
	leftUpForThisCell = NULL;
	leftDownForThisCell = NULL;
	rightUpForThisCell = NULL;
	rightDownForThisCell = NULL;

	x = -1;
	y = -1;
}
void Joint::draw() {
	ofNoFill();
	ofSetColor(0, 255, 0);
	ofCircle(pos.x, pos.y, JOINT_UNION_DISTANCE);
	ofSetColor(255, 0, 0);
	ofFill();
	ofDrawBitmapString(ofToString(x) + "," + ofToString(y), pos);
	ofSetColor(255, 255, 255);
}