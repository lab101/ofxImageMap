//
//  imapgeMap.cpp
//  imageMap
//
//  Created by Kris Meeusen on 31/05/12.
//  Copyright (c) 2012 Lab101.be. All rights reserved.
//

#include <iostream>
#include "imageMap.h"

imageMap::imageMap(){
    imagePath = "";
    mapfile= "";
    isToggle = false;
    isEnabled = false;
    lastActiveToggleButton = 0;    
	scale = 1;
}

imageMap::~imageMap(){
    //disable();
    mapTexture.clear();
}



void imageMap::setup(string newMapFile,string newImagePath){    
    setup( 0,0,newMapFile,newImagePath,false);
}

void imageMap::setup(float x,float y,string newMapFile,string newImagePath){    
    setup( x,y,newMapFile,newImagePath,false);
}


void imageMap::setup(float x,float y,string newMapFile,string newImagePath,bool toggle){    
    setup();
    position.set(x, y);
    isToggle = toggle;
    loadXML(newMapFile);
    loadImage(newImagePath);
}

void imageMap::setup(){    
    enable();
}

void imageMap::loadXML(string newMapFile){
    boxes.clear();
    if( XML.loadFile(newMapFile) ){
        mapfile = newMapFile;
		cout << newMapFile<< "loaded!\n";               
        XML.pushTag("settings");
        isToggle = XML.getValue("toggle", false);
        
        XML.pushTag("boxes");
        int numBoxes = XML.getNumTags("box");
        
        for(int i = 0; i < numBoxes; i++){
            string boxData = XML.getValue("box","0,0,0,0",i);
                        
            vector<string> data = ofSplitString(boxData, ",");
            
            mapItem rect;
            rect.pressing = false;
            rect.command =  XML.getAttribute("box", "command", "",i);
            
            rect.set(ofToFloat(data[0]), ofToFloat(data[1]), ofToFloat(data[2]), ofToFloat(data[3]));
            boxes.push_back(rect);
        }
        
        XML.popTag();
        XML.popTag();
        
        rebuildMesh();
        
    }else{
        ofLogError("unable to load" + newMapFile + "check data/ folder \n");
	}
}


void imageMap::saveXML(){
    // WRITE BOXES
    if(!XML.pushTag("settings"))
    {
        XML.setValue("settings", "");
        XML.pushTag("settings");
        XML.setValue("toggle", isToggle);
    }
    
    if(!XML.pushTag("boxes"))
    {
        XML.setValue("boxes", "");
        XML.pushTag("boxes");
    }
    
    int numBoxes = XML.getNumTags("box");
    cout << "removing " << numBoxes << " box nodes\n";
    
    for(int i = 0; i < numBoxes; i++){
        XML.removeTag("box",0);
    }
    
    for(int i=0;i< boxes.size();i++)
    {
        string boxData =  ofToString(boxes[i].x) + "," + ofToString(boxes[i].y) + "," + ofToString(boxes[i].width) + "," + ofToString(boxes[i].height);
        
        XML.setValue("box",boxData,i);
        XML.setAttribute("box", "command", boxes[i].command,i);
    }
    XML.popTag();
    XML.popTag();
    
    string pathToSave = getXMLFileName();
    cout << "path to save " << pathToSave << "\n";
    
    if(pathToSave == ""){
        pathToSave = getImageFileName();   
        cout << "path to save " << pathToSave << "\n";
        
        if(pathToSave.find("png") != string::npos) ofStringReplace(pathToSave, "png","xml");
        if(pathToSave.find("jpg") != string::npos) ofStringReplace(pathToSave, "jpg","xml");
    }       
    
    cout << "path to save " << pathToSave << "\n";
    XML.saveFile(pathToSave);


}


void imageMap::loadImage(string newImagePath){
    mapTexture.clear();

    // loading texture
    ofImage imageLoader;
    imageLoader.setUseTexture(false);
    if(!imageLoader.loadImage(newImagePath))
    {                
        cout <<  "! ofxTextureLoader : error loading image " << newImagePath << "\n";
        return;            
    }
    
    imagePath = newImagePath;

    
    if(!mapTexture.isAllocated())  mapTexture.allocate(imageLoader.width, imageLoader.height, ofGetGlInternalFormat(imageLoader.getPixelsRef()));
    
    mapTexture.loadData(imageLoader.getPixelsRef());
    height = mapTexture.getHeight() /2;
    width = mapTexture.getWidth() / 2;

}

void imageMap::rebuildMesh(){
    mapMesh.clear();
    
    for(vector<mapItem>::iterator it = boxes.begin();it <boxes.end();++it)
    {
        mapMesh.addVertex(ofVec3f(it->x,it->y + it->height,0));
        mapMesh.addVertex(ofVec3f(it->x,it->y,0));
        mapMesh.addVertex(ofVec3f(it->x + it->width,it->y,0));
        
        mapMesh.addVertex(ofVec3f(it->x,it->y + it->height,0));
        mapMesh.addVertex(ofVec3f(it->x+ it->width,it->y+ it->height,0));
        mapMesh.addVertex(ofVec3f(it->x + it->width,it->y,0));

        
        float offset= (it->pressing) ? mapTexture.getHeight() /2.0 : 0;
        
        mapMesh.addTexCoord(ofVec2f(it->x,it->y + it->height + offset));
        mapMesh.addTexCoord(ofVec2f(it->x,it->y + offset));
        mapMesh.addTexCoord(ofVec2f(it->x + it->width,it->y + offset));        
        
        mapMesh.addTexCoord(ofVec2f(it->x,it->y + it->height + offset));
        mapMesh.addTexCoord(ofVec2f(it->x+ it->width,it->y+ it->height + offset));
        mapMesh.addTexCoord(ofVec2f(it->x + it->width,it->y + offset));
    }
}

void imageMap::mouseReleased(ofMouseEventArgs& args){
    if(isToggle){
     handleToggleRelease(args);
    }else{
     handleNormalRelease(args);
    }
}
    
void imageMap::handleToggleRelease(ofMouseEventArgs& args){
        bool hasHit=false;
        
        for (vector<mapItem>::iterator it= boxes.begin(); it < boxes.end(); ++it) {
            ofRectangle translated = translateToWorldCord(*it);
            
            if(translated.inside(ofVec2f(args.x,args.y)) && it->pressing)
            {
                hasHit=true;
                ofNotifyEvent(onClick,it->command,this);
                     
                    if(lastActiveToggleButton !=0) lastActiveToggleButton->pressing = false;
                    lastActiveToggleButton = &(*it);
                   // it->pressing =true;
                
                break;
            }
        }    
        
            if(!hasHit){
                for (vector<mapItem>::iterator it2= boxes.begin(); it2 < boxes.end(); ++it2) {
                    it2->pressing =false;
                }  
                if(lastActiveToggleButton !=0) lastActiveToggleButton->pressing = true;
                
            }
            
            rebuildMesh();
               
    }
    
    void imageMap::handleNormalRelease(ofMouseEventArgs& args){
        for (vector<mapItem>::iterator it= boxes.begin(); it < boxes.end(); ++it) {
            ofRectangle translated = translateToWorldCord(*it);
            
            if(translated.inside(ofVec2f(args.x,args.y)) && it->pressing)
            {
                ofNotifyEvent(onClick,it->command,this);
            }
            it->pressing = false;
        }
        rebuildMesh(); 
    }



	ofRectangle imageMap::translateToWorldCord(ofRectangle rect){
			ofRectangle translated = rect;
			translated.x *= scale;
			translated.y *= scale;
			translated.width *= scale;
			translated.height *= scale;

			translated.x += position.x;
			translated.y += position.y;

			return translated;
	
	}

void imageMap::mousePressed(ofMouseEventArgs& args){
    bool doRebuildMesh = false;
    
    for (vector<mapItem>::iterator it= boxes.begin(); it < boxes.end(); ++it) {
        ofRectangle translated = translateToWorldCord(*it);
        
        if(translated.inside(ofVec2f(args.x,args.y)))
           {               
               it->pressing = true;
               doRebuildMesh = true;

		//	   cout << args.x << ":" << args.y << "\n";
		//	   		cout << translated.x << ": trans x" << (position.x * scale) << "\n";
		//cout << translated.y << "\n";

		//cout << "******************************\n\n";

           }
    }
    
    if (doRebuildMesh)rebuildMesh();
}

string imageMap::getXMLFileName(){
    return mapfile;    
}

string imageMap::getImageFileName(){
    return imagePath;    
}


void imageMap::draw(){
    ofSetColor(255);
    ofPushMatrix();
		ofTranslate(position);
		ofScale(scale,scale);
        mapTexture.bind();
        mapMesh.draw();
        mapTexture.unbind();
    ofPopMatrix();
}

void imageMap::drawFullImage()
{  
    ofSetColor(255);
    ofPushMatrix();
    ofTranslate(position);
    mapTexture.draw(0,0); 
    ofPopMatrix();
}


void imageMap::enable(){
    if (isEnabled) {
        return;
    }
    ofAddListener(ofEvents.mouseReleased,this,&imageMap::mouseReleased);
    ofAddListener(ofEvents.mousePressed,this,&imageMap::mousePressed);
    isEnabled = true;
}

void imageMap::disable(){
    ofRemoveListener(ofEvents.mouseReleased,this,&imageMap::mouseReleased);
    ofRemoveListener(ofEvents.mousePressed,this,&imageMap::mousePressed);
    isEnabled = false;

    
}