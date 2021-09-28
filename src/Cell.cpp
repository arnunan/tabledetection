#include "Cell.h"

Cell::Cell(ofRectangle rect) {
	this->rect = rect;
}

Cell::Cell() {
	initJointsPtr();
}

void Cell::draw(ofColor clr) {
	ofSetColor(clr);
	ofRect(rect);
	ofSetColor(0, 0, 255);

	string strX = "x:[";
	for (size_t i = 0; i < x.size(); i++)
		strX += ofToString(ofToString(x[i]) + ",");
	strX += "]";
	string strY = "y:[";
	for (size_t i = 0; i < y.size(); i++)
		strY += ofToString(ofToString(y[i]) + ",");
	strY += "]";

	ofDrawBitmapString(strX, rect.position + glm::vec3(10, 5, 0));
	ofDrawBitmapString(strY, rect.position + glm::vec3(10, 10, 0));

	ofSetColor(255, 255, 255);

	if (leftUp)leftUp->draw();
	if (leftDown)leftDown->draw();
	if (rightUp)rightUp->draw();
	if (rightDown)rightDown->draw();

	//DrawBitmap нумерации €чеек
}

void Cell::initJointsPtr() {
	leftUp = NULL;
	leftDown = NULL;
	rightUp = NULL;
	rightDown = NULL;
}