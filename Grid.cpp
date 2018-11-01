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
    rows = 14;
    cols = 10;
    initGrid(example);
}

Grid::~Grid(){
}

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

glm::uvec2 Grid::tileCoord(glm::vec3 isect) {
    float r = std::floor(isect.x + 0.5f);
    float c = std::floor(isect.y + 0.5f);
    bool negative = (r<0.0f || c<0.0f);
    bool outOfRange = (r>=rows || c>=cols);
    if (negative || outOfRange) {
        //click is negative and impossible or is greater than dims of row and cols of given map
        return glm::uvec2(-1, -1);
    }
    return glm::uvec2(r, c);
}

//getter function
char Grid::at(glm::uvec2 coord){
    if (coord.x>rows || coord.x<0 || coord.y>cols || coord.y<0) {
        return 1; //err return
    }
    char item = map[coord.x][coord.y];
    return item;
}
