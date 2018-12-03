
#ifndef UI_AGENT_H
#define UI_AGENT_H

#include "Inventory.h"
#include "Map.h"
#include "draw_text.hpp"
#include "Signalable.hpp"


class UI : public Magpie::Signalable{
public:
    //constructor
    UI(int object_id, int group_id);

    //handling signals sent to UI to change what is displayed
    void consume_signal() override;

    //the function handles state changes between Inventory and Map.
    //when "m" is pressed, the inventory is put away if it is out and map takes up whole screen.
    //when "i" is pressed, the map is put away if it is out and inventory scrolls out from the right.
    //when the inventory is done scrolling in there is a signal sent to statechanger causing
    //its state to be set to IDLE
    void stateChanger(char keyPressed);

    //draw function to draw all aspects of UI
    void drawUI(Scene::Camera const *camera);

    //if endGame is true it will trigger countdown for magpie escape
    bool endGame = false;

    //signal handler fields
    int objID;
    int grpID; 

    //misc fields
    Map map;
    Inventory inventory;
    uint32_t secureLvl = 1;
    uint32_t displayScore = 0;

};

#endif //UI_AGENT_H