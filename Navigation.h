//
// Created by 何宇 on 2018/10/11.
//

#include <vector>

#ifndef MAGPIE_NAVIGATION_H
#define MAGPIE_NAVIGATION_H

class Navigation {
    void loadGrid(Grid* map);
    Path* findPath(Grid::Coordinate from, Grid::Coordinate to);
};

class Path {
public:
    Path(std::vector<Grid::Coordinate>);
    Grid::Coordinate* next();
    void reset_iterator();

private:
    std::vector<Grid::Coordinate> path;
};

#endif //MAGPIE_NAVIGATION_H
