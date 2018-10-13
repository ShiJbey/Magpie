//
// Created by 何宇 on 2018/10/11.
//
#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <vector>

class Grid {
public:
    class Coordinate{
        //Coordinate();
        //Coordinate(uint32_t x, uint32_t y);
        public:
            uint32_t x;
            uint32_t y;
    };

    // Grid is initialized afterward.
    Grid();
    ~Grid();

    //number of rows and cols in grid set here
    uint32_t rows;
    uint32_t cols;

    // Init the grid.
    // Maybe loading from some other files?
    void initGrid(std::string floorPlan);

    // Grid is only readable. It can't be modified after being initialized
    // Grid grid;
    // grid.at(Coordinate(x, y)) 
    char at(Coordinate coor);

private:
    std::vector< std::vector< char > > map;
    //char map[rows][cols];
};

