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
	ofDirectory texDir;
	texDir.listDir("backgrounds");

	for (int i = 0; i < (int)texDir.size(); i++) {

		ofTexture t;
		ofLoadImage(t, texDir.getPath(i));

		backgrounds.push_back(t);

	}

	timeSinceBGChange = 0;
	activeBG = 0;
	currentBGTrans = 0.0;


	ofDirectory ghostTexDir;
	ghostTexDir.listDir("textures");

	for (int i = 0; i < (int)ghostTexDir.size(); i++) {

		ofTexture t;
		ofLoadImage(t, ghostTexDir.getPath(i));

		ghostTextures.push_back(t);

	}
	currentGhostTex = 0;

	gui = make_shared<Gui>();
	gui->setup();


	int numGhosts = 6;
	for (int i = 0; i < numGhosts; i++) {

		Ghost g;
		g.setup();
		g.getGui(gui);

		int texNum = i % ghostTextures.size();

		g.setTexture(ghostTextures[texNum]);

		//g.getFluid(fluid);
		ghosts.push_back(g);
		
	}

	bShowGui = true;

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


	//handle background transitions
	float time = ofGetElapsedTimef() - timeSinceBGChange;
	if (time < gui->bgFadeTime) {

		//fade in
		currentBGTrans = ofxeasing::map_clamp(time, 0.0, gui->bgFadeTime, 0.0f, gui->bgMaxTrans, &ofxeasing::linear::easeIn);
		
	}
	else if (time < gui->bgDuration - gui->bgFadeTime) {

		currentBGTrans = gui->bgMaxTrans;

	}
	else if (time < gui->bgDuration) {

		//fade out
		currentBGTrans = ofxeasing::map_clamp(time, gui->bgDuration - gui->bgFadeTime, gui->bgDuration, gui->bgMaxTrans, 0.0f, &ofxeasing::linear::easeIn);

	}
	else {
		timeSinceBGChange = ofGetElapsedTimef();
		activeBG++;

		if (activeBG >= backgrounds.size()) activeBG = 0;

	}
		


}

//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(0);

	//kinect.getBodySource()->drawProjected(0, 0, COLOR_WIDTH, COLOR_HEIGHT, ofxKFW2::ProjectionCoordinates::ColorCamera);

	ofEnableAlphaBlending();

	ofSetColor(255, 255 * currentBGTrans);
	backgrounds[activeBG].draw(0, 0, ofGetWidth(), ofGetHeight());


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

	if (bShowGui) {

		ofSetColor(255);
		ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate(), 2), 10, 20);

		gui->draw(10, 50);
	}

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if (key == 'f') ofToggleFullscreen();

	if (key == 's') gui->saveSettings();
	if (key == 'l') gui->loadSettings();
	if (key == 'g') bShowGui = !bShowGui;


	if (key == OF_KEY_RIGHT) {

		currentGhostTex++;

		if (currentGhostTex >= ghostTextures.size()) currentGhostTex = 0;

		for (int i = 0; i < ghosts.size(); i++) {
			ghosts[i].setTexture(ghostTextures[currentGhostTex]);
		}

	}
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
