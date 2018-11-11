
#ifndef UI_AGENT_H
#define UI_AGENT_H

#include "Agent.h"

class UI : public Agent{
public:
    //constructor
    UI(int object_id, int group_id);

    //handling signals sent to UI for triggering endgame
    void consumeSignal() override;

    //the function handles state changes between Inventory and Map.
    //when "m" is pressed, the inventory is put away if it is out and map takes up whole screen.
    //when "i" is pressed, the map is put awaay if it is out and inventory scrolls out from the right.
    //when the inventory is done scrolling in there is a signal sent to statechanger causing
    //its state to be set to IDLE
    void stateChanger(char keyPressed);

    //draw function to draw all aspects of UI
    void drawUI();

    //if endGame is true it will trigger countdown for magpie escape
    bool endGame = false;

    //signal handler fields
    int objID;
    int grpID; 

    //misc fields
    Map map;
    Inventory inventory;
    bool DONESCROLLING = true; //inventory starts off IDLE

};

#endif //UI_AGENT_H