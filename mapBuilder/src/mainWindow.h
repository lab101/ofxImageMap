#pragma once

#include "ofMain.h"
#include "ofxImageMap.h"


class mainWindow : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void clicked(string &args);		
        void configClicked(string &args);
   
private:
    ofxImageMap map;
    ofxImageMap config;
    
	ofxXmlSettings XML;
    mapItem activeRectangle;
    ofVec2f configAreaOffset;
    
    enum state {CONFIG,TEST,REMOVE};
    state currentState;
    
    ofRectangle txtCommand;
    string* selectedCommand;
    
    string debugTxt;

};
