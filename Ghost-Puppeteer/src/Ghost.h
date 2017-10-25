#pragma once

#include "ofMain.h"
#include "Cloth.hpp"
#include "PointMass.hpp"
#include "ofxKinectForWindows2.h"
#include "ofxFluid.h"

struct SmoothedJoint {

	deque<ofVec2f> positions;
	ofVec2f pos = ofVec2f(0);

	void update(ofVec2f newPos, int num) {
		positions.push_back(newPos);
		while (positions.size() > num) {
			positions.pop_front();
		}
		pos.set(0);
		for (int i = 0; i < positions.size(); i++) {
			pos += positions[i];
		}
		pos /= positions.size();
	}
};


class Ghost {

public:

	Ghost();

	void setup();
	void update();
	void update(ofxKFW2::Data::Body *body);
	void updateGuiVars();
	
	void draw();
	
	void getGui(const shared_ptr<Gui> g);
	//void getFluid(const shared_ptr<ofxFluid> f);
	
	
	shared_ptr<Gui> gui;

	Cloth cloth;

	SmoothedJoint rightHandTip, 
		rightHand, 
		rightElbow, 
		rightShoulder, 
		head, 
		leftShoulder, 
		leftElbow, 
		leftHand, 
		leftHandTip;

	bool bHaveBody, bFirstBodySetup;

	int numFramesToLock;

	vector<ofVec2f> pts;
	ofPolyline clothLine;

	ofTexture ghostTex;

	//shared_ptr<ofxFluid> fluid;



};