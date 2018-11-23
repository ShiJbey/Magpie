
#include "Map.h"
#include <iostream>


Map::Map(){

}

Map::~Map(){

}

void Map::unlockArea() {
    ;
}

void Map::drawMap() {
    if (state == ON) { //only draw map if STATE == ON
        std::cout<<"Map is out!"<<std::endl;
    }
}