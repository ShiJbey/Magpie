//
// Created by 何宇 on 2018/10/11.
//
#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

class Grid {
public:
    // Grid is initialized afterward.
    Grid();
    Grid(uint32_t rows_, uint32_t columns_);
    ~Grid();
    
    //number of rows and cols in grid set here
    uint32_t rows;
    uint32_t cols;

    // Grid is only readable. It can't be modified after being initialized
    // Grid grid;
    char at(glm::uvec2 coor);

    glm::uvec2 tileCoord(glm::vec3 isect);

    std::vector< std::vector< bool > > map;
    std::vector< std::vector< bool > > interaction_map;    
};

