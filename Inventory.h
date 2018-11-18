#pragma once

struct Inventory {

    //constructor
    Inventory();
    //destructor
    ~Inventory();

    //draw inventory
    void drawInv();
    
    //three states, OUT for rolling out, IN for rolling in and IDLE for not interacted
    enum STATE{
        OUT,
        IN,
        IDLE
    };
    //set initial state of inventory to idle
    enum STATE state = IDLE;

};

