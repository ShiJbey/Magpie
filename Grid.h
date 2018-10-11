//
// Created by 何宇 on 2018/10/11.
//
#pragma once

#include <cstdint>

class Grid {
public:
    class Coordinate{
        Coordinate();
        Coordinate(uint32_t x, uint32_t y);
    };

    // Grid is initialized afterward.
    Grid();
    ~Grid();

    // Init the grid.
    // Maybe loading from some other files?
    void initGrid();

    // Grid is only readable. It can't be modified after being initialized
    // Grid grid;
    // grid.at(Coordinate(x, y)) 
    char at(Coordinate coor);

private:
    char** map;
};

