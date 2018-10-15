//
// Created by 何宇 on 2018/10/11.
//

#include "Navigation.h"

Path::Path(std::vector<Grid::Coordinate> path) {
    this->path = path;
}

Grid::Coordinate* Path::next() {
    if (it == path.end()) return nullptr;
    return &(*it++);
}

void Path::reset_iterator() {
    it = path.begin();
}

void Navigation::loadGrid(Grid *map) {
    this->map = map;
}

Path* Navigation::findPath(Grid::Coordinate from, Grid::Coordinate to) {
    std::vector<Grid::Coordinate> path;

    uint32_t x_direction = from.x - to.x;
    uint32_t y_direction = from.y - to.y;
    
    return nullptr;
}

