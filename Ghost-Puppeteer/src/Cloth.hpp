//
//  Cloth.hpp
//  Cloth_Sim
//
//  Created by Adiel Fernandez on 5/30/17.
//
//

#ifndef Cloth_hpp
#define Cloth_hpp

#include <stdio.h>

#endif /* Cloth_hpp */


#pragma once
#include "ofMain.h"
#include "PointMass.hpp"
#include "ofxGui.h"
#include "Gui.h"



class Cloth{
    
public:
    
    Cloth();
    void setup(ofVec2f pos, int numWidth, int numHeight, int res, int fr);
    void update();
    void draw();

    
    enum MODE{
        
        CURTAIN = 0,
        FLAG = 1,
        FREE = 2
        
    };
    
    
    void createCloth(MODE m = CURTAIN);
    void reset();
    void clear();

    void drawTexturedMesh();
    
    //-----PHYSICS-----
    long previousTime;
    long currentTime;
    
    int desiredFrameRate;
    int fixedDeltaTime;
    float fixedDeltaTimeSeconds;
    
    int leftOverDeltaTime;
    
    int constraintAccuracy;

//    const float curtainTearSensitivity = 50; // distance the PointMasss have to go before ripping
    
    // every PointMass within this many pixels will be influenced by the cursor
    float mouseInfluenceSize;
    // minimum distance(sq) for tearing when user is right clicking
    float mouseTearSize;
    
    //amount we'll be pushing particles 
    float mouseInflueceStrength;
    //time step corrected influence amount
    float mouseInfluenceScalar;
    
    float tearDistance = 2.0;
    
    // amount to accelerate everything downward
    float gravity = 980;
    float damping = 0.99;
    float stiffness = 1;
    
    float waterDamping;
    float waterlevel;
    bool useWaterEffect;
    
    bool useWind;
    float windStrength;
    float windVariation;
    float windVariationSpeed;
    float windScale;
    
    bool useGravity;
    
    //upper left corner
    ofVec2f curtainPos;
    
    //Dimensions
    int numX;
    int numY;
    
    //point spacing
    int restDistance;
    
    bool drawLinks;
    bool drawPoints;
    bool allowTearing = false;
    bool showStressColors;
    
    vector<shared_ptr<PointMass>> pointMasses;
    
    //graphics
    ofVboMesh mesh;
    ofTexture tex;
    
    ofFloatColor compressCol;
    ofFloatColor stretchCol;
    ofFloatColor baseCol;
    
	
};



