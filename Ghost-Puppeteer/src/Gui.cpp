#include "Gui.h"



Gui::Gui(){

}


Gui::~Gui(){

}

void Gui::setup(){


	guiName = "settings";
	guiFileName = guiName + ".xml";
	gui.setup(guiName, guiFileName);
	gui.add(backgroundLabel.setup("   BACKGROUND SETTINGS", ""));
	gui.add(bgMaxTrans.setup("Background Trans", 0.1, 0.0, 1.0));
	gui.add(bgFadeTime.setup("Fade In Out Time", 1.0, 0.2, 4.0));
	gui.add(bgDuration.setup("Background Duration", 10.0, 10.0, 30.0));

	//shader settings
	gui.add(shaderLabel.setup("   FOG SHADER", ""));
	gui.add(fogLayers.setup("Fog Layers", 3, 1, 10));
	gui.add(driftSpeed.setup("Drift Speed", 0.1, 0.0, 1.0));
	gui.add(noiseSpeed.setup("Noise Speed", 0.08, 0.00001, 0.5));
	gui.add(noiseScale.setup("Noise Scale", 0.0008, 0.00001, 0.005));
	gui.add(ySquish.setup("Squish Y", 5.0, 0.0, 20.0));
	gui.add(octaveDiff.setup("Octave Diff", 0.1, 0.000001, 0.005));
	gui.add(fogTrans.setup("Fog Trans", 0.5, 0.0, 1.0));
	gui.add(fogMidpoint.setup("Fog Midpoint", 0.5, 0.0, 1.0));
	gui.add(fogBoundSharpness.setup("Boundary Sharpness", 6.0, 0.0, 20.0));

	//cloth settings
	gui.add(clothSettingsLabel.setup("   CLOTH SETTINGS", ""));
	gui.add(constraintAccuracySlider.setup("Solving Iterations", 3, 1, 10));
	gui.add(stiffnessSlider.setup("Link Stiffness", 1.0, 0.0, 1.2));
	gui.add(restDistSlider.setup("Rest Distance", 10.0, 1.0, 100.0));
	gui.add(mouseRadSlider.setup("Mouse Rad", 40, 0, 200));
	gui.add(mouseStrengthSlider.setup("Mouse Strength", 1.0, 0.0, 5.0));
	gui.add(dampingSlider.setup("Damping", 0.99, 0.85, 1.0));
	gui.add(showStressColorsToggle.setup("Show Stresses", false));
	gui.add(drawTexturedToggle.setup("Draw Textured", false));


	//forces
	gui.add(forcesLabel.setup("   FORCES", ""));
	gui.add(useGravity.setup("Use Gravity", false));
	gui.add(gravitySlider.setup("Gravity Strength", 980, 0, 2000));
	gui.add(useWind.setup("Use Wind", false));
	gui.add(windStrengthSlider.setup("Wind Strength", 1000, 0, 2000));
	gui.add(windVariationSlider.setup("Wind Variation", 500, 0, 3000));
	gui.add(windVariationSpeed.setup("Wind Var. Speed", 1.0, 0.01, 3.0f));
	gui.add(windScaleSlider.setup("Wind Scale", 1.0, 0.001, 0.15f));

	//Kinect vars
	gui.add(kinectLabel.setup("   KINECT", ""));
	gui.add(numSkeletonSmoothing.setup("Num Points to Smooth", 3, 1, 30));

	//Fluid vars
	//gui.add(fluidLabel.setup("   FLUID", ""));
	//gui.add(dissipation.setup("diss", 0.99, 0.9, 1.05));
	//gui.add(velDissipation.setup("vel diss", 0.99, 0.9, 1.05));
	//gui.add(gravity.setup("gravity", 0.0, -0.012, 0.012));


	//formatting
	gui.setHeaderBackgroundColor(255);



	//color applies to gui title only
	gui.setDefaultTextColor(ofColor(255));

	//color of the labels
	backgroundLabel.setDefaultTextColor(ofColor(0));
	backgroundLabel.setBackgroundColor(255);

	shaderLabel.setDefaultTextColor(ofColor(0));
	shaderLabel.setBackgroundColor(255);

	clothSettingsLabel.setDefaultTextColor(ofColor(0));
	clothSettingsLabel.setBackgroundColor(255);

	forcesLabel.setDefaultTextColor(ofColor(0));
	forcesLabel.setBackgroundColor(255);

	kinectLabel.setDefaultTextColor(ofColor(0));
	kinectLabel.setBackgroundColor(255);

	fluidLabel.setDefaultTextColor(ofColor(0));
	fluidLabel.setBackgroundColor(255);

	lastSave = -10.0f;
	lastLoad = -10.0f;

	loadSettings();



}


void Gui::draw(int x, int y){

	gui.setPosition(x, y);
	gui.draw();

	if (ofGetElapsedTimef() - lastSave < 1.0f) {
		ofSetColor(0, 255, 0);
		ofDrawBitmapString("Settings Saved", ofGetWidth() - 150, 20 );
	}
	if (ofGetElapsedTimef() - lastLoad < 1.0f) {
		ofSetColor(0, 128, 255);
		ofDrawBitmapString("Settings Loaded", ofGetWidth() - 150, 20);
	}
}

void Gui::saveSettings() {
	gui.saveToFile(guiFileName);
	lastSave = ofGetElapsedTimef();
}

void Gui::loadSettings() {
	gui.loadFromFile(guiFileName);
	lastLoad = ofGetElapsedTimef();
}