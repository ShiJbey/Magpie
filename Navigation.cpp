//
// Created by 何宇 on 2018/10/11.
//

#include "Navigation.h"

/**
 *  The Implementations of class Path.
 */

Path::Path(std::vector<glm::uvec2> path) {
    this->path = std::move(path);
}

glm::uvec2* Path::next() {
    if (it == path.end()) return nullptr;
    return &(*it++);
}

void Path::reset_iterator() {
    it = path.begin();
}


/**
 *  The Implementations of class Navigation.
 */

void Navigation::loadGrid(Grid *map) {
    this->map = map;
}

Path* Navigation::findPath(glm::uvec2 from, glm::uvec2 to) {
    std::vector<glm::uvec2> path_vector;

    int x_direction = ((to.x - from.x) > 0)?1:-1;
    int y_direction = ((to.y - from.y) > 0)?1:-1;

    glm::uvec2 t = from;

    path_vector.push_back(t);

    while (t.x != to.x) {
        t.x += x_direction;
        path_vector.push_back(t);
    }

    while (t.y != to.y) {
        t.y += y_direction;
        path_vector.push_back(t);
    }

    return new Path(path_vector);
}

