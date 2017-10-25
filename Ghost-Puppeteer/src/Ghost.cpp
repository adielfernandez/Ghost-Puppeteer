#include "Ghost.h"

Ghost::Ghost() {

}

void Ghost::setup() {
	//rest distance doesn't really matter, it will be adjusted later

	ofImage img;
	img.load("textures/bloody.jpg");

	ghostTex = img.getTexture();

	cloth.tex = ghostTex;
	cloth.setup(ofVec2f(400, 100), 60, 40, 20, 60);
	bHaveBody = false;
	bFirstBodySetup = false;

	


}

void Ghost::getGui(const shared_ptr<Gui> g) {
	gui = g;
}

//void Ghost::getFluid(const shared_ptr<ofxFluid> f) {
//	fluid = f;
//}

void Ghost::updateGuiVars() {
	cloth.mouseInflueceStrength = gui->mouseStrengthSlider;
	cloth.mouseInfluenceSize = gui->mouseRadSlider;
	cloth.stiffness = gui->stiffnessSlider;
	//cloth.restDistance = gui->restDistSlider;
	cloth.damping = gui->dampingSlider;
	cloth.mouseTearSize = gui->mouseTearRadSlider;
	cloth.gravity = gui->gravitySlider;
	cloth.constraintAccuracy = gui->constraintAccuracySlider;

	cloth.useGravity = gui->useGravity;
	cloth.gravity = gui->gravitySlider;

	cloth.useWind = gui->useWind;
	cloth.windStrength = gui->windStrengthSlider;
	cloth.windVariation = gui->windVariationSlider;
	cloth.windVariationSpeed = gui->windVariationSpeed;
	cloth.windScale = gui->windScaleSlider;
	cloth.showStressColors = gui->showStressColorsToggle;
}

//if we're updating without a body being passed as a parameter,
//there's no body found so let's pretend the body has
//left and fade the ghost away gracefully
void Ghost::update() {
	updateGuiVars();
	clothLine.clear();
	//cloth.update();
	

	bHaveBody = false;
	bFirstBodySetup = false;
}


//this will be called if the kinect sees a body to use
void Ghost::update(ofxKFW2::Data::Body *body) {

	updateGuiVars();

	bHaveBody = true;

	//pin the top of the curtain to the body between the joints,
	//essentially hanging a curtain from the points between:
	//right hand --- right elbow ---right shoulder---neck---left shoulder---left elbow---left hand
	
	//compute the total distance along the chain and evenly distribute the pin points
	//along the path and pin them to the skeleton each frame. Use ofPolyline as a shortcut
	float dist = 0;

	pts.clear();

	
	//body contains std::map of joints indexed by joint type
	//get iterator to joint 
	for (auto joint : body->joints) {
		
		if (joint.first == JointType_HandTipRight) {
			rightHandTip.update(joint.second.getPositionInDepthMap(), gui->numSkeletonSmoothing);
		}
		else if (joint.first == JointType_HandRight) {
			rightHand.update(joint.second.getPositionInDepthMap(), gui->numSkeletonSmoothing);
		}
		else if (joint.first == JointType_ElbowRight) {
			rightElbow.update(joint.second.getPositionInDepthMap(), gui->numSkeletonSmoothing);
		}
		else if (joint.first == JointType_ShoulderRight) {
			rightShoulder.update(joint.second.getPositionInDepthMap(), gui->numSkeletonSmoothing);
		}
		else if (joint.first == JointType_Head) {
			head.update(joint.second.getPositionInDepthMap(), gui->numSkeletonSmoothing);
		}
		else if (joint.first == JointType_ShoulderLeft) {
			leftShoulder.update(joint.second.getPositionInDepthMap(), gui->numSkeletonSmoothing);
		}
		else if (joint.first == JointType_ElbowLeft) {
			leftElbow.update(joint.second.getPositionInDepthMap(), gui->numSkeletonSmoothing);
		}
		else if (joint.first == JointType_HandLeft) {
			leftHand.update(joint.second.getPositionInDepthMap(), gui->numSkeletonSmoothing);
		}
		else if (joint.first == JointType_HandTipLeft) {
			leftHandTip.update(joint.second.getPositionInDepthMap(), gui->numSkeletonSmoothing);
		}
	}


	clothLine.clear();

	//draw directly at points
	//clothLine.addVertex(rightHandTip);
	//clothLine.addVertex(rightHand);
	//clothLine.addVertex(rightElbow);
	//clothLine.addVertex(rightShoulder);
	//clothLine.addVertex(neck);
	//clothLine.addVertex(leftShoulder);
	//clothLine.addVertex(leftElbow);
	//clothLine.addVertex(leftHand);
	//clothLine.addVertex(leftHandTip);

	//use curveTo
	clothLine.curveTo(rightHandTip.pos);
	clothLine.curveTo(rightHand.pos);
	clothLine.curveTo(rightElbow.pos);
	clothLine.curveTo(rightShoulder.pos);
	clothLine.curveTo(head.pos);
	clothLine.curveTo(leftShoulder.pos);
	clothLine.curveTo(leftElbow.pos);
	clothLine.curveTo(leftHand.pos);
	clothLine.curveTo(leftHandTip.pos);

	
	//change the rest distance of the cloth so it doesn't go crazy with tension/expansion
	cloth.restDistance = clothLine.getPerimeter() / (float)(cloth.numX - 1);

	if ( !bFirstBodySetup ) {
		cloth.reset();
	
		ofLog() << "-----Resetting cloth-----";

		//show some info about the body while we're here
		ofLog() << "TrackingID: " << body->trackingId;
		

	}

	//we have the polyline now, so go through the cloth and attach the points to it
	//loop through the first row of pointMasses and attach them to the polyLine points
	//also go through the rest of them and assign them to be at the clothLine position
	//to prevent chaos as the cloth snaps to a distant body
	for (int i = 0; i < cloth.pointMasses.size(); i++) {

		//
		float pct = (i % cloth.numX) / (float)(cloth.numX - 1);
		
		//first row
		if (i < cloth.numX) {
			cloth.pointMasses[i]->pinPos = clothLine.getPointAtPercent(pct);
		}

		//if it's the first time we've found a body, position all the points manually
		//by assigning the points to the shoulder line and every row below it dropped down
		//by the rest distance
		if ( !bFirstBodySetup ) {
			int rowNum = (i - i%cloth.numX) / cloth.numX;
			ofVec2f newPos = clothLine.getPointAtPercent(pct) + ofVec2f(0, cloth.restDistance * rowNum);

			cloth.pointMasses[i]->pos = newPos;
		}


	}

	bFirstBodySetup = true;


	//add fluid points to ghost
	//fluid->addTemporalForce(head.pos, ofVec2f(0.0, -1.0), ofFloatColor(1.0, 1.0, 1.0, 0.5), 10.0f);




	cloth.update();


}

void Ghost::draw() {

	if (bHaveBody) {



		ofPushStyle();

		ofSetLineWidth(1);
		ofSetColor(255, 100);
		

		if (gui->drawTexturedToggle) {
			cloth.drawTexturedMesh();
		} else {
			cloth.draw();
		}



		//draw sensing area bounding box
		//ofNoFill();
		//ofSetLineWidth(1);
		//ofSetColor(255);
		//ofDrawRectangle(0, 0, depthWidth, depthHeight);

		//draw polyline as a guide
		//ofSetColor(255, 0, 255);
		//ofSetLineWidth(10);
		//clothLine.draw();


		ofPopStyle();

	}


}
