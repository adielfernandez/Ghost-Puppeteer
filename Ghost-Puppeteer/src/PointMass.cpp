//
//  PointMass.cpp
//  Cloth_Sim
//
//  Created by Adiel Fernandez on 6/1/17.
//
//

#include "PointMass.hpp"


PointMass::PointMass(){
    
}

void PointMass::setup(int xPos, int yPos, float rest){
    
    pos.set(xPos, yPos);
    lastPos = pos;
    acc.set(0, 0);
    
    mass = 1.0;
    restDist = rest;


    
}

void PointMass::applyForce(float fX, float fY){
    acc += ofVec2f(fX, fY)/mass;
}

void PointMass::pin(){
    bIsPinned = true;
    pinPos = pos;
}

void PointMass::pinTo(float pX, float pY){
    bIsPinned = true;
    pinPos.set(pX, pY);
}

void PointMass::updatePhysics(float timeStep, float damping){
    
//    applyForce(0, mass * gravity);
    
    ofVec2f vel = pos - lastPos;
    
    // dampen velocity
    vel *= damping; //good damping value = 0.99
    
    float timeStepSq = timeStep * timeStep;
    
    // calculate the next position using Verlet Integration

    ofVec2f nextPos = pos + vel + 0.5 * acc * timeStepSq;
    
    // reset variables
    lastPos = pos;
    
    pos = nextPos;
    
    acc.set(0);
    
}
    
void PointMass::updateInteractions(float mouseInfluenceSize, float mouseInfluenceScalar, float mouseTearSize){
    
    // this is where our interaction comes in.
    float distanceSquared = distPointToSegmentSquared(ofGetPreviousMouseX(), ofGetPreviousMouseY(), ofGetMouseX(), ofGetMouseY(), pos.x, pos.y);
    
    //left mouse button
    if ( ofGetMousePressed(0) ) {
        
        if (distanceSquared < mouseInfluenceSize * mouseInfluenceSize) {

            // To change the velocity of our PointMass, we subtract that change from the lastPosition.
            // When the physics gets integrated (see updatePhysics()), the change is calculated
            // Here, the velocity is set equal to the cursor's velocity
            lastPos.x = pos.x - (ofGetMouseX() - ofGetPreviousMouseX()) * mouseInfluenceScalar;
            lastPos.y = pos.y - (ofGetMouseY() - ofGetPreviousMouseY()) * mouseInfluenceScalar;
        }
            
    }
    
    //if( ofGetMousePressed( 2 ) ){
    //    // if the right mouse button is clicking, we tear the cloth by removing links
    //    if (distanceSquared < mouseTearSize * mouseTearSize) links.clear();
    //}
    
    
    
}

void PointMass::solveConstraints(float restD, float stiff, float tearDist, bool allowTearing){
    /* Link Constraints */
    // Links make sure PointMass connected to this one is at a set distance away
    bool bIsTorn = false;
	restDist = restD;

    for (int i = 0; i < links.size(); i++) {
        
        // calculate the distance between the two PointMasss
        ofVec2f diff = links[i].p1 -> pos - links[i].p2 -> pos;
        float d = diff.length(); //sqrt(diff.x * diff.x + diff.y * diff.y);
        
        // find the difference, or the ratio of how far along the restingDistance the actual distance is.
        links[i].stretchRatio = (restDist - d) / d;
        
        // if the distance is more than curtainTearSensitivity, the cloth tears
        if (allowTearing && d > tearDist){
            bIsTorn = true;
            break;
        }

        
        // Inverse the mass quantities
        float im1 = 1 / links[i].p1 -> mass;
        float im2 = 1 / links[i].p2 -> mass;
        float scalarP1 = (im1 / (im1 + im2)) * stiff;
        float scalarP2 = stiff - scalarP1;
        
        // Push/pull based on mass
        // heavier objects will be pushed/pulled less than attached light objects
        links[i].p1 -> pos.x += diff.x * scalarP1 * links[i].stretchRatio;
        links[i].p1 -> pos.y += diff.y * scalarP1 * links[i].stretchRatio;
        
        links[i].p2 -> pos.x -= diff.x * scalarP2 * links[i].stretchRatio;
        links[i].p2 -> pos.y -= diff.y * scalarP2 * links[i].stretchRatio;
        
    }
    
    if( bIsTorn ){
        links.clear();
    }
    
    /* Boundary Constraints */
    // These if statements keep the PointMasss within the screen
	int width = 1920;
	int height = 1080;
    if (pos.y < 1)
        pos.y = 2 * (1) - pos.y;
    if (pos.y > height -1)
        pos.y = 2 * (height - 1) - pos.y;
    
    if (pos.x > width-1)
        pos.x = 2 * (width - 1) - pos.x;
    if (pos.x < 1)
        pos.x = 2 * (1) - pos.x;
    
    /* Other Constraints */
    // make sure the PointMass stays in its place if it's pinned
    if (bIsPinned) {
        pos = pinPos;
    }
}


// Using http://www.codeguru.com/forum/showpost.php?p=1913101&postcount=16
// We use this to have consistent interaction
// so if the cursor is moving fast, it won't interact only in spots where the applet registers it at
float PointMass::distPointToSegmentSquared(float lineX1, float lineY1, float lineX2, float lineY2, float pointX, float pointY) {
    float vx = lineX1 - pointX;
    float vy = lineY1 - pointY;
    float ux = lineX2 - lineX1;
    float uy = lineY2 - lineY1;
    
    float len = ux*ux + uy*uy;
    float det = (-vx * ux) + (-vy * uy);
    if ((det < 0) || (det > len)) {
        ux = lineX2 - pointX;
        uy = lineY2 - pointY;
        return min(vx*vx+vy*vy, ux*ux+uy*uy);
    }
    
    det = ux*vy - uy*vx;
    return (det*det) / len;
}



