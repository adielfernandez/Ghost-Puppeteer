//
//  Cloth.cpp
//  Cloth_Sim
//
//  Created by Adiel Fernandez on 5/30/17.
//
//

#include "Cloth.hpp"



Cloth::Cloth(){
    
    
    
}

void Cloth::setup(ofVec2f pos, int numWidth, int numHeight, int res, int fr){
    
    //dimensioning
    curtainPos = pos;
    numX = numWidth;
    numY = numHeight;
    restDistance = res;
    desiredFrameRate = fr;
    
    //-----Physics-----
    
    //time in milliseconds between frames (running at desired frame rate)
    fixedDeltaTime = (int)(1000/(float)desiredFrameRate);
    fixedDeltaTimeSeconds = (float)fixedDeltaTime / 1000.0;
    leftOverDeltaTime = 0;
    constraintAccuracy = 1;
    
    stretchCol.set(0, 0.5, 1.0);
    baseCol.set(1.0);
    compressCol.set(1.0, 0, 0);
    
	useGravity = true;
	useWaterEffect = false;
	useWind = false;



    
    createCloth();
    
}

void Cloth::clear(){
    
    pointMasses.clear();
    mesh.clear();
    
}

void Cloth::reset(){
    
    clear();
    createCloth();
    
}

void Cloth::createCloth(MODE m){

    //resize the vector to the final size so calls to
    //push_back/re-allocations do not invalidate pointers
    //to elements that are stored in the link
    pointMasses.resize(numY * numX);
    
    
    //counter for keeping track of the element we're on
    int current = 0;
    
    for(int y = 0; y < numY; y++){
        for(int x = 0; x < numX; x++){
            
            //set the point mass at each grid point: position * link distance + curtain pos
			pointMasses[current] = make_shared<PointMass>();
            pointMasses[current]->setup(x * restDistance + curtainPos.x, y * restDistance + curtainPos.y, restDistance);
            
            
            if (x != 0){

                int left = current - 1;
                
                Link l;
                l.p1 = pointMasses[current];
                l.p2 = pointMasses[left];
                
                pointMasses[current]->links.push_back(l);
                
            }
            
            // the index for the PointMass are one dimensions,
            // so we convert x,y coordinates to 1 dimension using the formula y*width+x
            if (y != 0){

                int above = (y - 1) * numX + x;
                
                Link l;
                l.p1 = pointMasses[current];
                l.p2 = pointMasses[above];
                
                pointMasses[current]->links.push_back(l);
                
            }
            
            //PIN AT DIFFERENT PLACES TO CREATE DIFFERENT SCENARIOS

            if( m == CURTAIN ){
                
                if (y == 0){
                    pointMasses[current]->bIsPinned = true;
                    pointMasses[current]->pinPos = pointMasses[current]->pos;
                }
                
            } else if( m == FLAG ){

                if (x == 0){
                    pointMasses[current]->bIsPinned = true;
                    pointMasses[current]->pinPos = pointMasses[current]->pos;
                }
                
            } else if( m == FREE ){
                
                //do nothing
                
            }
            
            //pin the four corners
//            if ( (y == 0 && x == 0) || (y == 0 && x == numX-1) || (x == 0 && y == numY-1) || (x == numX-1 && y == numY-1)){
//                pointMasses[current].bIsPinned = true;
//                pointMasses[current].pinPos = pointMasses[current].pos;
//            }
            
            
            //go through all the points and add them into a mesh,
            //linking up all the points with the others in a triangle mesh
            
            /*
             
             0_____1__...__Nx
             |    /|
             |   / |
             |  /  |
             | /   |
             |/____|
            3|    /|2
             |   / |
             |  /  |
             | /   |
             |/____|
             .
             .
             .
             Ny
             
             */
            
            
            //dont do for the right side or bottom row
            if( x < numX - 1 && y < numY - 1 ){
                
                //translate the x and y into the index numbers
                int i0 = (y    ) * numX + (x    );
                int i1 = (y    ) * numX + (x + 1);
                int i2 = (y + 1) * numX + (x + 1);
                int i3 = (y + 1) * numX + (x    );
                
                //add the two triangles into the indices
                mesh.addIndex(i0);
                mesh.addIndex(i1);
                mesh.addIndex(i3);
                
                mesh.addIndex(i1);
                mesh.addIndex(i2);
                mesh.addIndex(i3);
                
            }
            
            //set texture coordinates
            float textureResX = tex.getWidth()/(float)numX;
            float textureResY = tex.getHeight()/(float)numY;
            
            mesh.addTexCoord(ofVec2f(x * textureResX, y * textureResY));
            
            
            //add the point to the mesh
            mesh.addVertex(pointMasses[current]->pos);
            
            
            //increment the counter
            current++;
            
        }
    }
    
    
    
    
}

void Cloth::update(){
    
    // calculate elapsed time
    currentTime = ofGetElapsedTimeMillis();
    long deltaTimeMS = currentTime - previousTime;
    
    previousTime = currentTime; // reset previous time
    
    // break up the elapsed time into manageable chunks
    int timeStepAmt = (int)( (deltaTimeMS + leftOverDeltaTime) / (float)fixedDeltaTime );
    
    // limit the timeStepAmt to prevent potential freezing
    timeStepAmt = min(timeStepAmt, 5);
    
    // store however much time is leftover for the next frame
    leftOverDeltaTime = (int)deltaTimeMS - (timeStepAmt * fixedDeltaTime);
    
    // How much to push PointMasses when the user is interacting
    mouseInfluenceScalar = mouseInflueceStrength / timeStepAmt;
    
    //clear the mesh vertices
    mesh.clearVertices();
    mesh.clearColors();
    
    //update physics
    
    //outer for-loop will iterate through as many frames as we need in order to catch up
    //with the actual framerate. Example: if we're running slow, we'll run this more times
    //to make up for the animation data we're missing between app frames. If it's too soon
    //before desired framerate, we'll won't run at all.
    for (int iteration = 1; iteration <= timeStepAmt; iteration++) {

        // solve the constraints multiple times
        // the more it's solved, the more accurate.
        for (int x = 0; x < constraintAccuracy; x++) {
            for (int i = 0; i < pointMasses.size(); i++) {
				pointMasses[i]->solveConstraints( restDistance, stiffness, tearDistance, allowTearing);
            }
        }
        
        // update each PointMass's position
        float t = ofGetElapsedTimef();
        
        for (int i = 0; i < pointMasses.size(); i++) {
            
            pointMasses[i]->updateInteractions(mouseInfluenceSize, mouseInfluenceScalar, mouseTearSize);
            
            
            if( useWind ){
                
                float wind = windStrength + windVariation * ofSignedNoise(t * windVariationSpeed, pointMasses[i]->pos.y * windScale);
                pointMasses[i]->applyForce(wind, 0);
            }
            
            if( useGravity ){
                pointMasses[i]->applyForce(0, gravity);
            }
            
            
            //are we above or below the water line
            if( useWaterEffect && pointMasses[i]->pos.y > waterlevel ){
                pointMasses[i]->updatePhysics(fixedDeltaTimeSeconds, waterDamping);
            } else {
                pointMasses[i]->updatePhysics(fixedDeltaTimeSeconds, damping);
            }
            
        }

    }
    
    
    //// update each PointMass's position
    for (int i = 0; i < pointMasses.size(); i++) {
        mesh.addVertex(pointMasses[i]->pos);
        
        
        if( showStressColors ){
            
            //add a color for that point equivalent to the avg amount of stretch in the links
            if( pointMasses[i]->links.size() > 0 ){
                
                float stretch = 0;
                
                for( int j = 0; j < pointMasses[i]->links.size(); j++){
                    stretch += pointMasses[i]->links[j].stretchRatio;
                }
                
                stretch /= pointMasses[i]->links.size();
                
                
                ofFloatColor c;
                
                //negative stretch means its stretching, postive = compression
                if( stretch < 0.0 ){
                    
                    //stretch goes from 0 to -1.0
                    c = baseCol.getLerped(stretchCol, -stretch);
                } else {
                    //compression goes from 0 to 1.0
                    //much more subtle so map more aggressively
                    float f = ofMap(stretch, 0.0, 0.2, 0.0, 1.0, true);
                    c = baseCol.getLerped(compressCol, stretch);
                }
                
                mesh.addColor(c);
                
            }
            
        } else {
            mesh.addColor(ofFloatColor(1.0));
        }
        
        
    }
    
    
}

void Cloth::drawTexturedMesh(){
    
    ofSetColor(255);
    
    tex.bind();
    
    mesh.draw();
    
    tex.unbind();
    
    
}



void Cloth::draw(){
    
    ofSetColor(255);
    mesh.drawWireframe();
    
    
    
//    for(int i = 0; i < pointMasses.size(); i++){
//        
//        //draw links
//        ofSetColor(255, 100);
//        for( int j = 0; j < pointMasses[i].links.size(); j++){
//            
//            ofVec2f p1 = pointMasses[i].links[j].p1 -> pos;
//            ofVec2f p2 = pointMasses[i].links[j].p2 -> pos;
//            
//            ofDrawLine(p1, p2);
//        }
//        
//        
//        //draw points
//        if( pointMasses[i].bIsPinned ){
//            ofSetColor(255, 0, 0);
//        } else {
//            ofSetColor(255);
//        }
//        ofDrawRectangle(pointMasses[i].pos.x - 1.5, pointMasses[i].pos.y - 1.5, 3, 3);
//        
//        
//    }
    
}








