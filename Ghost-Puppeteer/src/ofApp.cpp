#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetFrameRate(60);
	ofSetVerticalSync(true);



	kinect.open();
	kinect.initDepthSource();
	//kinect.initColorSource();
	//kinect.initInfraredSource();
	kinect.initBodySource();
	//kinect.initBodyIndexSource();


	shader.load("shaders/smoke");
	shader.setMillisBetweenFileCheck(200);

	//fluid = make_shared<ofxFluid>();
	//fluid->allocate(ofGetWidth(), ofGetHeight());

	//fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

	//fbo.begin();
	//ofClear(0.0, 0.0, 0.0, 0.0);
	//fbo.end();

	//backgrounds 
	ofImage img;
	img.load("backgrounds/graveyard.jpg");
	backgrounds.push_back(img);

	gui = make_shared<Gui>();
	gui->setup();


	int numGhosts = 6;
	for (int i = 0; i < numGhosts; i++) {

		Ghost g;
		g.setup();
		g.getGui(gui);
		//g.getFluid(fluid);
		ghosts.push_back(g);
		
	}

}

//--------------------------------------------------------------
void ofApp::update(){
	kinect.update();

	auto bodies = kinect.getBodySource()->getBodies();


	bool bodyFound = false;

	for (int i = 0; i < bodies.size(); i++) {
		if ( bodies[i].tracked ) {
			bodyFound = true;
			ghosts[i].update( &bodies[i] );
		}
		else {
			ghosts[i].update();
		}
	}


	//if( !bodyFound) {
	//	ghosts[0].update();
	//}

	//fluid->dissipation = gui->dissipation;
	//fluid->velocityDissipation = gui->velDissipation;
	//fluid->setGravity(ofVec2f(0.0, gui->gravity));


	////add gui settings to cloths

	//fluid->update();

}

//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(0);

	//kinect.getBodySource()->drawProjected(0, 0, COLOR_WIDTH, COLOR_HEIGHT, ofxKFW2::ProjectionCoordinates::ColorCamera);

	ofEnableAlphaBlending();

	ofSetColor(255, 255 * gui->bgTrans);
	backgrounds[0].draw(0, 0, ofGetWidth(), ofGetHeight());


	int depthWidth = 512;
	int depthHeight = 424;

	//scale to fit the height 
	float scale = ofGetHeight() / (float)depthHeight;

	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2 - (depthWidth * scale) / 2.0, 0);
	ofScale(scale, scale);
	
	//fbo.begin();
	//ofClear(0.0, 0.0, 0.0, 0.0);
	//
	//fluid->draw();
	//fbo.end();

	//ofSetColor(255, 100);
	//fbo.draw(0, 0);

	for (int i = 0; i < ghosts.size(); i++) {
		ghosts[i].draw();
	}

	ofPopMatrix();

	shader.begin();{
		//we want to pass in some varrying values to animate our type / color
		shader.setUniform1f("time", ofGetElapsedTimef());

		//we also pass in the mouse position
		shader.setUniform2f("mouse", mouseX, ofGetHeight() - mouseY);
		shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
		shader.setUniform1f("fogLayers", gui->fogLayers);
		shader.setUniform1f("driftSpeed", gui->driftSpeed);
		shader.setUniform1f("noiseSpeed", gui->noiseSpeed);
		shader.setUniform1f("noiseScale", gui->noiseScale);
		shader.setUniform1f("ySquish", gui->ySquish);
		shader.setUniform1f("fogTrans", gui->fogTrans);
		shader.setUniform1f("fogMidpoint", gui->fogMidpoint);
		shader.setUniform1f("fogSharpness", gui->fogBoundSharpness);
		shader.setUniform1f("octaveDiff", gui->octaveDiff);


		ofSetColor(30);
		ofFill();
		ofDrawRectangle(-1000, -1000, ofGetWidth() + 2000, ofGetHeight() + 2000);
	}shader.end();


	//ofSetColor(255, 200, 0);
	//float x = ofGetWidth() / 2 + 400 * cos(ofGetElapsedTimef() * 1.5);
	//float y = ofGetHeight() / 2 + 400 * sin(ofGetElapsedTimef() * 1.5);
	//ofDrawCircle(x, y, 100);


	ofSetColor(255);
	ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate(), 2), 10, 20);

	gui->draw(10, 50);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if (key == 'f') ofToggleFullscreen();

	if (key == 's') gui->saveSettings();
	if (key == 'l') gui->loadSettings();

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
