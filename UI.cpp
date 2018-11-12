
#include "Inventory.h"
#include "Map.h"
#include "UI.h"


UI::UI(int object_id, int group_id) {
    objID = object_id;
    grpID = group_id;

    //initialize map
    map = Map();
    //initialize inventory
    inventory = Inventory();
}

/*
void UI::consumeSignal(){
    //get right signals
    Signal *s = SignalQueue::getInstance().get(objID, grpID); 
    while (s != NULL) {
        std::string parsedSig = s->msg;
        if (parsedSig == "endgame") {
            endGame = true;
        }
        s = SignalQueue::getInstance().get(objID, grpID);
    }
}
*/

void UI::stateChanger(char keyPressed) {
    if (keyPressed == 'i') {
        //set state of inventory to OUT and state of map to OFF
        inventory.state = Inventory::OUT;
        map.state = Map::OFF;
    }
    else if (keyPressed == 'm') {
        //set state of inventory to IN and state of map to ON
        inventory.state = Inventory::IN;
        map.state = Map::ON;
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


