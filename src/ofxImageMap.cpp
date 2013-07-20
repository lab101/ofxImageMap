//
//  imapgeMap.cpp
//  imageMap
//
//  Created by Kris Meeusen on 31/05/12.
//  Copyright (c) 2012 Lab101.be. All rights reserved.
//

#include <iostream>
#include "ofxImageMap.h"

ofxImageMap::ofxImageMap(){
    imagePath = "";
    mapfile= "";
    isEnabled = false;
    lastActiveToggleButton = 0;    
}

ofxImageMap::~ofxImageMap(){
    mapTexture.clear();
}



void ofxImageMap::setup(string newImagePath){    
    setup( 0,0,newImagePath,false);
}


void ofxImageMap::setup(float x,float y,string newImagePath,bool toggle){    
    setup();
    position.set(x, y);
    loadFromImage(newImagePath);

}

void ofxImageMap::setup(){    
    enable();
}

void ofxImageMap::loadXML(string newMapFile){
    boxes.clear();
    
    if(!ofFile(newMapFile).exists())
        return;
    
    if( XML.loadFile(newMapFile) ){
        mapfile = newMapFile;
		cout << newMapFile<< " loaded"  << endl;
        XML.pushTag("settings");
        
        XML.pushTag("boxes");
        int numBoxes = XML.getNumTags("box");
        
        for(int i = 0; i < numBoxes; i++){
            string boxData = XML.getValue("box","0,0,0,0",i);
            //bool isToggle =
            
            vector<string> data = ofSplitString(boxData, ",");
            
            mapItem rect;
            rect.isPressed = false;
            rect.isToggle = XML.getAttribute("box","toggle", false,i);
            
            std::cout << rect.isToggle << std::endl;
            rect.command =  XML.getAttribute("box", "command", "",i);
            
            rect.set(ofToFloat(data[0]), ofToFloat(data[1]), ofToFloat(data[2]), ofToFloat(data[3]));
            boxes.push_back(rect);
        }
        
        XML.popTag();
        XML.popTag();
        
        rebuildMesh();
        
    }else{
        ofLogError("unable to load " + newMapFile + "check data/ folder \n");
	}
}


void ofxImageMap::saveXML(){
    // WRITE BOXES
    if(!XML.pushTag("settings"))
    {
        XML.setValue("settings", "");
        XML.pushTag("settings");
    }
    
    if(!XML.pushTag("boxes"))
    {
        XML.setValue("boxes", "");
        XML.pushTag("boxes");
    }
    
    int numBoxes = XML.getNumTags("box");
    cout << "removing " << numBoxes << " box nodes" << endl;
    
    for(int i = 0; i < numBoxes; i++){
        XML.removeTag("box",0);
    }
    
    for(int i=0;i< boxes.size();i++)
    {
        string boxData =  ofToString(boxes[i].x) + "," + ofToString(boxes[i].y) + "," + ofToString(boxes[i].width) + "," + ofToString(boxes[i].height);
        
        XML.setValue("box",boxData,i);
        XML.setAttribute("box", "toggle",  boxes[i].isToggle,i);
        XML.setAttribute("box", "command", boxes[i].command,i);
    }
    
    XML.popTag();
    XML.popTag();
    
    
    XML.saveFile(getXMLFileName(imagePath));


}


void ofxImageMap::loadFromImage(string newImagePath){
    mapTexture.clear();

    // loading texture
    ofImage imageLoader;
    imageLoader.setUseTexture(false);
    if(!imageLoader.loadImage(newImagePath))
    {                
        cout <<  "! ofxTextureLoader : error loading image " << newImagePath  << endl;
        return;            
    }
    
    imagePath = newImagePath;

    
    if(!mapTexture.isAllocated())  mapTexture.allocate(imageLoader.width, imageLoader.height, ofGetGlInternalFormat(imageLoader.getPixelsRef()));
    
    mapTexture.loadData(imageLoader.getPixelsRef());
    height = mapTexture.getHeight() /2;
    width = mapTexture.getWidth();
    
    loadXML(getXMLFileName(newImagePath));


}

void ofxImageMap::rebuildMesh(){
    mapMesh.clear();
    
    for(vector<mapItem>::iterator it = boxes.begin();it <boxes.end();++it)
    {
        mapMesh.addVertex(ofVec3f(it->x,it->y + it->height,0));
        mapMesh.addVertex(ofVec3f(it->x,it->y,0));
        mapMesh.addVertex(ofVec3f(it->x + it->width,it->y,0));
        
        mapMesh.addVertex(ofVec3f(it->x,it->y + it->height,0));
        mapMesh.addVertex(ofVec3f(it->x+ it->width,it->y+ it->height,0));
        mapMesh.addVertex(ofVec3f(it->x + it->width,it->y,0));

        
        float offset= (it->isPressed) ? mapTexture.getHeight() /2.0 : 0;
        
        mapMesh.addTexCoord(ofVec2f(it->x,it->y + it->height + offset));
        mapMesh.addTexCoord(ofVec2f(it->x,it->y + offset));
        mapMesh.addTexCoord(ofVec2f(it->x + it->width,it->y + offset));        
        
        mapMesh.addTexCoord(ofVec2f(it->x,it->y + it->height + offset));
        mapMesh.addTexCoord(ofVec2f(it->x + it->width,it->y+ it->height + offset));
        mapMesh.addTexCoord(ofVec2f(it->x + it->width,it->y + offset));
        
    }
    
    //cout << "rebuild " << mapMesh.getVertices().size() << std::endl;

}

void ofxImageMap::mouseReleased(ofMouseEventArgs& args){
//    if(isToggle){
//        handleToggleRelease(args);
//    }else{
//        handleNormalRelease(args);
//    }
    
    bool hasHit=false;
    
    for (vector<mapItem>::iterator it= boxes.begin(); it < boxes.end(); ++it) {
        
        ofRectangle translated = *it;
        translated.x += position.x;
        translated.y += position.y;
        
        if(translated.inside(ofVec2f(args.x,args.y)) && it->isPressed)
        {
            hasHit=true;
             ofNotifyEvent(onClick,it->command,this);
            
            if(it->isToggle){
              //  it->isPressed = !it->isPressed;
              //  rebuildMesh();


            }else{
                it->isPressed = false;
                rebuildMesh();
            }
            
        }
    }

}

void ofxImageMap::setPosition(ofVec2f newPosition){
    position = newPosition;
}


void ofxImageMap::handleToggleRelease(ofMouseEventArgs& args){
//        bool hasHit=false;
//        
//        for (vector<mapItem>::iterator it= boxes.begin(); it < boxes.end(); ++it) {
//            
//            ofRectangle translated = *it;
//            translated.x += position.x;
//            translated.y += position.y;
//            
//            if(translated.inside(ofVec2f(args.x,args.y)) && it->isPressed)
//            {
//                hasHit=true;
//                ofNotifyEvent(onClick,it->command,this);
//                     
//                    if(lastActiveToggleButton !=0) lastActiveToggleButton->isPressed = false;
//                    lastActiveToggleButton = &(*it);
//                
//                break;
//            }
//        }    
//        
//            if(!hasHit){
//                for (vector<mapItem>::iterator it2= boxes.begin(); it2 < boxes.end(); ++it2) {
//                    it2->isPressed =false;
//                }  
//                if(lastActiveToggleButton !=0) lastActiveToggleButton->isPressed = true;
//                
//            }
//            
//            rebuildMesh();
    
    }
    
    void ofxImageMap::handleNormalRelease(ofMouseEventArgs& args){
//        for (vector<mapItem>::iterator it= boxes.begin(); it < boxes.end(); ++it) {
//            ofRectangle translated = *it;
//            translated.x += position.x;
//            translated.y += position.y;
//            
//            if(translated.inside(ofVec2f(args.x,args.y)) && it->isPressed)
//            {
//                ofNotifyEvent(onClick,it->command,this);
//            }
//            it->isPressed = false;
//        }
//        rebuildMesh(); 
    }

void ofxImageMap::mousePressed(ofMouseEventArgs& args){
    bool doRebuildMesh = false;
    
    for (vector<mapItem>::iterator it= boxes.begin(); it < boxes.end(); ++it) {
        ofRectangle translated = *it;
        translated.x += position.x;
        translated.y += position.y;
        
        if(translated.inside(ofVec2f(args.x,args.y)))
           {
               if(it->isToggle) {
                   it->isPressed = !it->isPressed;
               }else{
                   it->isPressed = true;
               }
               doRebuildMesh = true;
           }
    }
    
    if (doRebuildMesh)rebuildMesh();
}

string ofxImageMap::getXMLFileName(string imagePath){
    
    string xmlFileName = imagePath;
    if(xmlFileName.find("png") != string::npos) ofStringReplace(xmlFileName, "png","xml");
    if(xmlFileName.find("jpg") != string::npos) ofStringReplace(xmlFileName, "jpg","xml");

    return xmlFileName;
}

string ofxImageMap::getImageFileName(){
    return imagePath;    
}


void ofxImageMap::draw(){
    ofSetColor(255);
    ofPushMatrix();
    ofTranslate(position);
        mapTexture.bind();
        mapMesh.draw();
        mapTexture.unbind();
    ofPopMatrix();
}

void ofxImageMap::drawFullImage()
{  
    ofSetColor(255);
    ofPushMatrix();
    ofTranslate(position);
    mapTexture.draw(0,0); 
    ofPopMatrix();
}


void ofxImageMap::enable(){
    if (isEnabled) {
        return;
    }
    ofAddListener(ofEvents().mouseReleased,this,&ofxImageMap::mouseReleased);
    ofAddListener(ofEvents().mousePressed,this,&ofxImageMap::mousePressed);
    isEnabled = true;
}

void ofxImageMap::disable(){
    ofRemoveListener(ofEvents().mouseReleased,this,&ofxImageMap::mouseReleased);
    ofRemoveListener(ofEvents().mousePressed,this,&ofxImageMap::mousePressed);
    isEnabled = false;

    
}