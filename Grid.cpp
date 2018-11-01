//
// Created by 何宇 on 2018/10/11.
//

#include "Grid.h"
#include <vector>
#include <iostream>

//hardcoded floorplan for prototype
std::string example = "prototype";

Grid::Grid() {
    rows = 0;
    cols = 0;
};

//grid cannot be changed after initialization
Grid::Grid(uint32_t rows_, uint32_t columns_){
    rows = rows_;
    cols = columns_;

    // Reserve space in the chars
    for (uint32_t i = 0; i < rows; i++) {
        std::vector< bool > row;
        // Set the default value to the empty string
        for (uint32_t j = 0; j < cols; j++) {
            row.push_back(false);
        }
        map.push_back(row);
    }

    // Reserve space for the entity pointers
    for (uint32_t i = 0; i < rows; i++) {
        std::vector< int* > row;
        // Set the default value to the empty string
        for (uint32_t j = 0; j < cols; j++) {
            row.push_back(nullptr);
        }
        interaction_map.push_back(row);
    }
}

Grid::~Grid(){
}

glm::uvec2 Grid::tileCoord(glm::vec3 isect) {
    float r = std::floor(isect.x);
    float c = std::floor(isect.y);
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
