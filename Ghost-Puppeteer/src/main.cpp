#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	//ofSetupOpenGL(1920,1080,OF_WINDOW);			// <-------- setup the GL context


	ofGLFWWindowSettings settings;
	settings.width = 1920;
	settings.height = 1080;
	settings.setGLVersion(2, 1); /// < select your GL Version here
	ofCreateWindow(settings);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
