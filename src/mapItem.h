//
//  mapItem.h
//  imageMap
//
//  Created by Kris Meeusen on 31/05/12.
//  Copyright (c) 2012 Lab101.be. All rights reserved.
//

#ifndef imageMap_mapItem_h
#define imageMap_mapItem_h

class mapItem : public ofRectangle
{
public:
    string command;  
    bool pressing;
};

#endif
