
#pragma once

#include <cstdint>
#include "Scene.hpp"


struct Inventory {

    //constructor
    Inventory();
    //destructor
    ~Inventory();

    //draw inventory
    void drawInv(Scene::Camera const *camera);

    //update inventory
    void updateInv(float elapse);
    
    //four states, OUT for rolling out, IN for rolling in,
    //IDLEIN for staying in, IDLEOUT for staying out
    enum STATE{
        IN,
        OUT,
        IDLEIN,
        IDLEOUT
    };
    //set initial state of inventory to idle
    enum STATE state = IDLEIN;

    //TODO: find a good val for these and tweak them
    float xOutLim = -2.0f;
    float xInLim = -1.0f;
    float delX = 0.5f; //the amount that inventory will slide in or out each time
    float scroll_countdown = 1.0f;

    uint32_t printouthelp = 1;
};
