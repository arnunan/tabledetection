#include "ofApp.h"

void ofApp::setup() {
	ofImage img;
	img.loadImage("Example.jpg");

	ps.skew(img.getPixelsRef());

	tpm.recognizePage(ps.images.back());
	scaleImage = 0.5;
	drawTableBlobs = false;
}

void ofApp::update() {
}

void ofApp::draw() {

	ofPushMatrix();

	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	ofScale(scaleImage, scaleImage);
	ofTranslate(-ofGetWidth() / 2, -ofGetHeight() / 2);
	ofTranslate(canvasPos);

	ps.draw(0, -ps.images[0].getHeight());
	tpm.draw(0, ps.images[0].getHeight(), drawTableBlobs);


	ofPopMatrix();
	ofSetColor(255, 255, 255);
}

void ofApp::keyPressed(int key) {
	if (key == ' ')
		drawTableBlobs = !drawTableBlobs;
}

void ofApp::keyReleased(int key) {
}

void ofApp::mouseMoved(int x, int y) {
}

void ofApp::mouseDragged(int x, int y, int button) {
	offsetPos = ofVec2f(x, y) - lastMousePos;
	lastMousePos = ofVec2f(x, y);
	canvasPos += offsetPos;
}

void ofApp::mousePressed(int x, int y, int button) {
	lastMousePos = ofVec2f(x, y);
}

void ofApp::mouseScrolled(ofMouseEventArgs& mouse) {
	scaleImage += mouse.scrollY*0.1;
	if (scaleImage < 0) scaleImage = abs(scaleImage);
}

void ofApp::mouseReleased(int x, int y, int button) {
}

void ofApp::mouseEntered(int x, int y) {
}

void ofApp::mouseExited(int x, int y) {
}

void ofApp::windowResized(int w, int h) {
}

void ofApp::gotMessage(ofMessage msg) {
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
	ofImage img;
	img.loadImage(dragInfo.files[0]);

	ps.skew(img.getPixelsRef());

	tpm.recognizePage(ps.images.back());
	scaleImage = 0.5;
}