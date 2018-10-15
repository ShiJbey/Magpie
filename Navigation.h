//
// Created by 何宇 on 2018/10/11.
//
#pragma once

#include <vector>
#include "Grid.h"

class Path {
public:
    Path(){}
    Path(std::vector<glm::uvec2> path);
    glm::uvec2* next();
    void reset_iterator();

private:
    std::vector<glm::uvec2> path;
    std::vector<glm::uvec2>::iterator it = path.begin();
};

class Navigation {
    void loadGrid(Grid* map);
    Path* findPath(glm::uvec2 from, glm::uvec2 to);

private:
    Grid* map;
};
