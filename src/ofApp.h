#pragma once

#include "ofMain.h"
#include "TablePageManager.h"
#include "Utils.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void mouseScrolled(ofMouseEventArgs& mouse);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		TablePageManager tpm;
		
		PageSkew	ps;

		ofVec2f canvasPos;
		ofVec2f offsetPos;
		ofVec2f lastMousePos;
		float   scaleImage;

		bool drawTableBlobs;
};


