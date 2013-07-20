//
//  imageMap.h
//  imageMap
//
//  Created by Kris Meeusen on 31/05/12.
//  Copyright (c) 2012 Lab101.be. All rights reserved.
//

#ifndef imageMap_imageMap_h
#define imageMap_imageMap_h

#include "ofmain.h"
#include "ofxXmlSettings.h"
#include "mapItem.h"

class ofxImageMap{
private:
	ofxXmlSettings XML;
    
    ofMesh mapMesh;
    ofTexture mapTexture;
    
    string mapfile;
    string imagePath;
    
    bool isEnabled;
    mapItem* lastActiveToggleButton;
    ofVec2f position;
    
    void handleToggleRelease(ofMouseEventArgs& args);
    void handleNormalRelease(ofMouseEventArgs& args);
	ofRectangle translateToWorldCord(ofRectangle rect);

    string getXMLFileName(string imagePath);
    void loadXML(string mapFile);

public:
    ofxImageMap();
    ~ofxImageMap();
    
    void setup(string newImagePath);
    void setup(float x,float y,string newImagePath);
    void setup(float x,float y,string newImagePath,bool toggle);
    void setup();
    
    void saveXML();
    void loadFromImage(string imagePath);
    void rebuildMesh();

    string getImageFileName();
    
    void setPosition(ofVec2f newPosition);
    
    void draw();
    void drawFullImage();
    
    void enable();
    void disable();
    
    void mouseReleased(ofMouseEventArgs& args);
    void mousePressed(ofMouseEventArgs& args);

    ofEvent<string> onClick ;
    
    vector<mapItem> boxes;
        
    float height;
    float width;
	float scale;
    bool isToggle;



};

#endif
