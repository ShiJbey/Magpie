
//#include "SignalQueue.h"
#include "UI.h"
#include <iostream>


UI::UI(int object_id, int group_id) {
    objID = object_id;
    grpID = group_id;

    //initialize map
    map = Map();
    //initialize inventory
    inventory = Inventory();
}

void UI::consume_signal() {
    /*
    //get right signals
    Magpie::Signal *s = Magpie::Signal(objID, grpID); 
    while (s != NULL) {
        std::string parsedSig = s->msg;
        if (parsedSig == "endgame") {
            endGame = true;
        }
        s = SignalQueue::getInstance().get(objID, grpID);
    }
    */
}

void UI::stateChanger(char keyPressed) {
    if (keyPressed == 'i') {
        //set state of inventory to OUT and state of map to OFF
        inventory.state = Inventory::OUT;
        map.state = Map::OFF;
        std::cout<<"state of map is now: "<<map.state<<std::endl;
    }
    else if (keyPressed == 'm') {
        //no matter if map is being called or dismissed inventory should not be out
        inventory.state = Inventory::IN;
        if (map.state == Map::OFF) {
            //set state of map to ON
            map.state = Map::ON;
        }
        else if (map.state == Map::ON) {
            //set state of map to OFF
            map.state = Map::OFF;
        }
        std::cout<<"state of map is now: "<<map.state<<std::endl;
    }
    else if (DONESCROLLING == true) { //d for done scrolling in
        //set state of inventory to IDLE
        inventory.state = Inventory::IDLE;
    }
}

void UI::drawUI() {
    inventory.drawInv();
    map.drawMap();
    { //draw everything else in UI
        //score
        ;
        if (endGame == false) { //security level
            ;
        }
        else if (endGame == true) { //endgame countdown
            ;
        }
    }
}


