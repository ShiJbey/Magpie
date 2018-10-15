//
// Created by 何宇 on 2018/10/11.
//
#pragma once

#include <cstdint>
#include <glm/glm.hpp>

class Grid {
public:

    // Grid is initialized afterward.
    Grid();
    ~Grid();

    // Init the grid.
    // Maybe loading from some other files?
    void initGrid();

    // Grid is only readable. It can't be modified after being initialized
    // Grid grid;
    // grid.at(Coordinate(x, y))
    char at(glm::uvec2 coor);

private:
    char** map;
};

