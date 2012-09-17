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

class imageMap{
private:
	ofxXmlSettings XML;
    
    ofMesh mapMesh;
    ofTexture mapTexture;
    void rebuildMesh();
    
    string mapfile;
    string imagePath;
    
    bool isEnabled;
    mapItem* lastActiveToggleButton;
    ofVec2f position;
    
    void handleToggleRelease(ofMouseEventArgs& args);
    void handleNormalRelease(ofMouseEventArgs& args);
	ofRectangle translateToWorldCord(ofRectangle rect);

    
public:
    imageMap();
    ~imageMap();
    
    void setup(string newMapFile,string newImagePath);
    void setup(float x,float y,string newMapFile,string newImagePath);
    void setup(float x,float y,string newMapFile,string newImagePath,bool toggle);
    void setup();
    
    void loadXML(string mapFile);
    void saveXML();
    void loadImage(string imagePath);
    
    string getXMLFileName();
    string getImageFileName();
    
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
