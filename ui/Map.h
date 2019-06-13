
#pragma once

#include "../base/Scene.hpp"


struct Map {

    //constructor
    Map();
    //destructor
    ~Map();

    //draw the map
    void drawMap(Scene::Camera const *camera);

    //two different states, either ON for on screen or OFF for offscreen
    enum STATE {
        ON,
        OFF
    };
    //initialize first state
    enum STATE state = OFF;

    //available areas on map, by default there is one area open
    uint32_t unlockedAreas = 1;
};