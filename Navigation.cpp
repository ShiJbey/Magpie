//
// Created by 何宇 on 2018/10/11.
//

#include "Navigation.h"

/**
 *  The Implementations of class Path.
 */

Path::Path(std::vector<glm::uvec2> path) {
    this->path = std::move(path);
    it = this->path.begin();
}

glm::uvec2 Path::next() {
    std::cout << "NEXT:" << it->x << it->y << std::endl;
    if (it == path.end()) return glm::uvec2(0, 0);
    return *(it++);
}

bool Path::isEmpty() {
    return it == path.end();
}

glm::uvec2 Path::top() {
    return *path.begin();
}

/**
 *  The Implementations of class Navigation.
 */

void Navigation::loadGrid(Grid *map) {
    this->map = map;
}

Path Navigation::findPath(glm::uvec2 from, glm::uvec2 to) {
    std::vector<glm::uvec2> path_vector;

    int x_direction = (to.x > from.x)?1:-1;
    int y_direction = (to.y > from.y)?1:-1;

    glm::uvec2 t = from;

    path_vector.push_back(t);

    while (t.x != to.x) {
        std::cout << "(" << t.x << "," << to.x << ")" <<  x_direction << std::endl;
        t.x += x_direction;
        path_vector.push_back(t);
    }

    while (t.y != to.y) {
        std::cout << "(" << from.y << "," << to.y << ")" << y_direction << std::endl;
        t.y += y_direction;
        path_vector.push_back(t);
    }

    for (glm::uvec2 p : path_vector) {
        std::cout << "(" << p.x << "," << p.y << ")" << std::endl;
    }

    return Path(path_vector);
}

