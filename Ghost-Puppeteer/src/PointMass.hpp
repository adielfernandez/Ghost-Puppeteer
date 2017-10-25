//
//  PointMass.hpp
//  Cloth_Sim
//
//  Created by Adiel Fernandez on 6/1/17.
//
//

#ifndef PointMass_hpp
#define PointMass_hpp

#include <stdio.h>

#endif /* PointMass_hpp */

#pragma once

#include "ofMain.h"


//forward declare Link
class PointMass;

struct Link{
    shared_ptr<PointMass> p1;
	shared_ptr<PointMass> p2;
    
    float stretchRatio;
    
};

class PointMass{
    
public:
    
    PointMass();
    
    void setup(int xPos, int yPos, float rest);
    void updatePhysics(float timeStep, float damping);
    void updateInteractions(float mouseInfluenceSize, float mouseInfluenceScalar, float mouseTearSize);
    void solveConstraints(float restD, float stiff, float tearSensitivity, bool allowTearing);
    void applyForce( float fX, float fY);
    void pinTo(float pX, float pY);
    void pin();
    
    
    //convenience method for getting interpolated
    //mouse Positions between frames
    float distPointToSegmentSquared(float lineX1, float lineY1, float lineX2, float lineY2, float pointX, float pointY);
    
    void draw();
    
    
    
    float restDist;
    
    
    ofVec2f pos, lastPos;
    ofVec2f acc;
    
    float mass;
    float damping;
    
    vector<Link> links;
    
    ofVec2f pinPos;
    bool bIsPinned = false;
    
};






