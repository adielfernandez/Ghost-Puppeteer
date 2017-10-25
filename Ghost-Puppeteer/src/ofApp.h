#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxAutoReloadedShader.h"
#include "ofxGui.h"
#include "Gui.h"
#include "Cloth.hpp"
#include "PointMass.hpp"
#include "Ghost.h"
#include "ofxFluid.h"

#define DEPTH_WIDTH 512
#define DEPTH_HEIGHT 424
#define DEPTH_SIZE DEPTH_WIDTH * DEPTH_HEIGHT

#define COLOR_WIDTH 1920
#define COLOR_HEIGHT 1080


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
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofxAutoReloadedShader shader;
		vector<ofImage> backgrounds;

		vector<Ghost> ghosts;
		//shared_ptr<ofxFluid> fluid;
		//ofFbo fbo;

		ofxKFW2::Device kinect;
		ICoordinateMapper* coordinateMapper;

		shared_ptr<Gui> gui;
		
};
