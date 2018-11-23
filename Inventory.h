
#ifndef INVENTORY_H
#define INVENTORY_H
struct Inventory {

    //constructor
    Inventory();
    //destructor
    ~Inventory();

    //draw inventory
    void drawInv();
    
    enum STATE { S_OUT, S_IN, S_IDLE };

    //set initial state of inventory to idle
    enum STATE state = S_IDLE;

};


#endif