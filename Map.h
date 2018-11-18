#pragma once

struct Map {

    //constructor
    Map();
    //destructor
    ~Map();
    
    //unlocks another area in the full floor plan map
    void unlockArea();

    //draw the map
    void drawMap();

    //two different states, either ON for on screen or OFF for offscreen
    enum STATE {
        ON,
        OFF
    };
    //initialize first state
    enum STATE state = OFF;

    //all the parts of the map

};