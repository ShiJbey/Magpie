//
// Created by 何宇 on 2018/10/11.
//

#include "Grid.h"
#include <vector>
#include <iostream>

//hardcoded floorplan for prototype
std::string example = "prototype";

//grid cannot be changed after initialization
Grid::Grid(){
    rows = 10;
    cols = 14;
    initGrid(example);
}

Grid::~Grid(){
}

//TODO: change
void Grid::initGrid(std::string floorPlan) {
    if (floorPlan == "prototype") {
        //fill in floor plan
        for (uint32_t i=0; i<rows; i++) {
            std::vector< char > r;
            for (uint32_t j=0; j<cols; j++) {
                if (i == 8 && j == 3) { //long table part 1
                    r.push_back('t');
                }
                else if (i == 8 && j == 4) { //long table part 2
                    r.push_back('t');
                }
                else {
                    r.push_back('e');
                }
            }
            map.push_back(r);
        }
    }
    else {
        std::cout<< "unexpected floorplan input" << std::endl;
    }
}

//getter function
char Grid::at(glm::uvec2 coord){
    if (coord.x>rows || coord.x<0 || coord.y>cols || coord.y<0) {
        return 1; //err return
    }
    char item = map[coord.x][coord.y];
    return item;
}
