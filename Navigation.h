//
// Created by 何宇 on 2018/10/11.
//
#pragma once

#include <vector>
#include "Grid.h"

class Path {
public:
    Path(){}
    Path(std::vector<Grid::Coordinate> path);
    Grid::Coordinate* next();
    void reset_iterator();

private:
    std::vector<Grid::Coordinate> path;
    std::vector<Grid::Coordinate>::iterator it = path.begin();
};

class Navigation {
    void loadGrid(Grid* map);
    Path* findPath(Grid::Coordinate from, Grid::Coordinate to);

private:
    Grid* map;
};
