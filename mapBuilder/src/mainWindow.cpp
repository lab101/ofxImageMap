#include "mainWindow.h"


//--------------------------------------------------------------
void mainWindow::setup(){
	ofSetFrameRate(60);
   // ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    
   
    config.setup(0,0,"config.png",true);
    ofAddListener(config.onClick,this,&mainWindow::configClicked);
    
    configAreaOffset.set(10, 100);
    
    map.setup(configAreaOffset.x,configAreaOffset.y, "keyboard.png",true);
    map.setPosition(configAreaOffset);
    ofAddListener(map.onClick,this,&mainWindow::clicked);
    
    currentState = TEST;
    
    txtCommand.set(650, 24, 200, 20);
    selectedCommand = 0;
    
    debugTxt = "";

}


void mainWindow::clicked(string &args){
    debugTxt = "clicked on: " + args;
//    cout << args;
   // cout << *args;
}

void mainWindow::configClicked(string &args){
        if(args == "REMOVE")
        {
            activeRectangle.set(0, 0, 0, 0);
            currentState = REMOVE;
            map.disable();

        }else if(args == "CONFIG"){
            activeRectangle.set(0, 0, 0, 0);
            currentState = CONFIG;
            map.disable();

        } else if(args == "TEST"){
            activeRectangle.set(0, 0, 0, 0);
            currentState = TEST;
            map.rebuildMesh();
            map.enable();
        }else if(args == "SAVE")
        {
            map.saveXML();
        }
    
    cout << "state changed to " << args << currentState << "\n";
}



//--------------------------------------------------------------
void mainWindow::update(){
}

//--------------------------------------------------------------
void mainWindow::draw(){
   
    // draw config bar background lines
    ofSetColor(120, 120, 120);
    ofRect(0, 0, ofGetWidth(), 70);
    ofSetColor(20, 20, 20);
    ofRect(0, 70, ofGetWidth(), 3);
    ofSetColor(255, 255, 255);
    
    config.draw();

    if (currentState == CONFIG || currentState == REMOVE) {
        map.drawFullImage();
    }else if(currentState == TEST){
        map.draw();
        ofDrawBitmapString(debugTxt, ofVec2f(750,58));

        return;
    }
    
    
    // Draw config boxes
    ofPushMatrix();
    
    ofTranslate(configAreaOffset);
    for (int i=0; i < map.boxes.size(); i++) {
        ofFill();
        ofSetColor(0, 255, 255,40);
        ofRect(map.boxes[i]);
        
        ofNoFill();
        ofSetColor(0, 255, 255,255);
        ofRect(map.boxes[i]);
        
        ofFill();
        
        ofSetColor(0, 0, 0,115);
        ofRect(map.boxes[i].x,map.boxes[i].y + 2,map.boxes[i].width,14);
        
        ofSetColor(255, 255, 255,255);
        ofDrawBitmapString(map.boxes[i].command, map.boxes[i].x +2,map.boxes[i].y + 12);
    }
    ofPopMatrix();

    
    ofSetColor(0);
    ofDrawBitmapString("command:", ofVec2f(650,18));

    if(activeRectangle.width > 0 && activeRectangle.height > 0){
        ofSetColor(0, 255, 255);
        ofNoFill();
        ofRect(activeRectangle);
    }
    
    //txt stuff
    ofFill();
    ofSetColor(255);
    ofRect(txtCommand);

    ofSetColor(0);
    if(selectedCommand !=0) ofDrawBitmapString(*selectedCommand, ofVec2f(650,38));

}




//--------------------------------------------------------------
void mainWindow::keyPressed(int key){

}

//--------------------------------------------------------------
void mainWindow::keyReleased(int key){

    if(selectedCommand != 0)
    {
        if(key == OF_KEY_BACKSPACE)
        {
            if(selectedCommand->size() > 0) (*selectedCommand).erase(selectedCommand->end()-1, selectedCommand->end());
        }else{
            (*selectedCommand) += key; 
        }
    }
  }

//--------------------------------------------------------------
void mainWindow::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void mainWindow::mouseDragged(int x, int y, int button){
    if (currentState != CONFIG) {
        return;
    }
    
    activeRectangle.width = x - activeRectangle.x;
    activeRectangle.height = y - activeRectangle.y;
}

//--------------------------------------------------------------
void mainWindow::mousePressed(int x, int y, int button){
    if (currentState == TEST) {
        return;
    }else if(currentState == CONFIG)
    {
        //cout << "recording" << endl ;

        activeRectangle.set(x , y, 0, 0);
    }
}

//--------------------------------------------------------------
void mainWindow::mouseReleased(int x, int y, int button){
  if(currentState == REMOVE)
    {
        for(vector<mapItem>::iterator it =map.boxes.begin();it != map.boxes.end();++it)
        {
            ofRectangle translated = *it;
            translated.x += configAreaOffset.x;
            translated.y += configAreaOffset.y;

            if(translated.inside(x,y)){
                it = map.boxes.erase(it);
                selectedCommand = 0;
                return;
            }        
        }
    }else if (currentState == CONFIG) {
        activeRectangle.width = x - activeRectangle.x;
        activeRectangle.height = y - activeRectangle.y;
        activeRectangle.x -= configAreaOffset.x;
        activeRectangle.y -= configAreaOffset.y;
        
        // choose between a selection or a new rectangle
        if (activeRectangle.width > 3 && activeRectangle.height > 3) {
            map.boxes.push_back(activeRectangle);    
            selectedCommand = &(map.boxes.end()-1)->command;
        }else {
            // check for a hit.
            for (int i=0; i < map.boxes.size(); i++) {
                ofRectangle translated = map.boxes[i];
                translated.x += configAreaOffset.x;
                translated.y += configAreaOffset.y;
                
                if(translated.inside(x,y)){
                    selectedCommand = &map.boxes[i].command;
                    return;
                }        
            }

        }
        
        activeRectangle.set(0, 0, 0, 0);
    }
}
    
 

//--------------------------------------------------------------
void mainWindow::windowResized(int w, int h){

}

//--------------------------------------------------------------
void mainWindow::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void mainWindow::dragEvent(ofDragInfo dragInfo){
    
    for (int i=0; i < dragInfo.files.size(); i++) {
        string filename = dragInfo.files[i];
        if (filename.find(".png") != string::npos || filename.find(".jpg") != string::npos) {
            cout << "loading image :" << filename << endl;
            currentState = CONFIG;
            selectedCommand = 0;
            map.loadFromImage(filename);
        }
    }
}