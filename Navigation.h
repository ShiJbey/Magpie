//
// Created by 何宇 on 2018/10/11.
//
#pragma once

#include <vector>
#include "Grid.h"

class Path {
public:
    Path(){}
    Path(std::vector<Grid::Coordinate>);
    Grid::Coordinate* next();
    void reset_iterator();

private:
    std::vector<Grid::Coordinate> path;
};

class Navigation {
    void loadGrid(Grid* map);
    Path* findPath(Grid::Coordinate from, Grid::Coordinate to);
};
