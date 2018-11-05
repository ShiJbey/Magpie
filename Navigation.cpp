//
// Created by 何宇 on 2018/10/11.
//

#include "Navigation.h"
#include <iostream>
#include <deque>
#include <map>
#include <tuple>
#include <algorithm>

/**
 *  The Implementations of class Path.
 */

Magpie::Path::Path(std::vector<glm::uvec2> path) {
    this->path = std::move(path);
    it = this->path.begin();
}

glm::uvec2 Magpie::Path::next() {
    std::cout << "NEXT:" << it->x << it->y << std::endl;
    if (it == path.end()) return glm::uvec2(0, 0);
    return *(it++);
}

bool Magpie::Path::isEmpty() {
    return it == path.end();
}

glm::uvec2 Magpie::Path::top() {
    return *path.begin();
}

std::vector<glm::uvec2> Magpie::Navigation::get_adjacent(glm::vec2 pos) {
    std::vector<glm::uvec2> adjacent_tiles;
    if (level->can_move_to((uint32_t)pos.x + 1U, (uint32_t)pos.y)) {
        adjacent_tiles.push_back(glm::uvec2((uint32_t)pos.x + 1U, (uint32_t)pos.y));
    }
    if (level->can_move_to((uint32_t)pos.x - 1U, (uint32_t)pos.y)) {
        adjacent_tiles.push_back(glm::uvec2((uint32_t)pos.x - 1U, (uint32_t)pos.y));
    }
    if (level->can_move_to((uint32_t)pos.x, (uint32_t)pos.y + 1U)) {
        adjacent_tiles.push_back(glm::uvec2(pos.x, (uint32_t)pos.y + 1U));
    }
    if (level->can_move_to((uint32_t)pos.x, (uint32_t)pos.y - 1U)) {
        adjacent_tiles.push_back(glm::uvec2((uint32_t)pos.x, (uint32_t)pos.y - 1U));
    }
    return adjacent_tiles;
};

/**
 *  The Implementations of class Navigation.
 */

void Magpie::Navigation::loadLevel(Magpie::MagpieLevel *level) {
    this->level = level;
}

Magpie::Path Magpie::Navigation::findPath(glm::vec2 from, glm::uvec2 to) {
    std::vector<glm::uvec2> path_vector;

    // Implement BFS for path finding
    // https://www.redblobgames.com/pathfinding/tower-defense/
    
    // Unexplored grid positions
    std::deque< glm::uvec2 > frontier;
    frontier.push_back(from);
    // Map a current position to the position previous
    std::vector< std::tuple<glm::uvec2, glm::uvec2> >came_from;
    // Maps position A to position B, where you traveled from position B
    // to get to position A
    came_from.push_back(std::make_tuple(from, glm::uvec2(-1U, -1U)));
    // Perform BFS
    while( !frontier.empty() ) {
        glm::uvec2 current = frontier.front();
        frontier.pop_front();

        if (current == to) {
            break;
        }

        std::vector<glm::uvec2> adjacent_tiles = get_adjacent(current);
        for (auto it = adjacent_tiles.begin(); it != adjacent_tiles.end(); it++) {
            bool visited = false;
            
            for (auto iter = came_from.begin(); iter != came_from.end(); iter++) {
                if (std::get<0>(*iter) == *it) {
                    break;
                    visited = true;
                }
            }

            if (!visited) {
                frontier.push_back(*it);
                came_from.push_back(std::make_tuple(*it, current));

                if (*it == to) {
                    break;
                }
            }
        }
    }

    // Reverse iterate to get a path
    glm::uvec2 current = to;
    while (current != glm::uvec2(-1U, -1U)) {
        path_vector.push_back(current);
        for (auto iter = came_from.begin(); iter != came_from.end(); iter++) {
            // Find the entry for this position in 'came_from'
            // and set the current position to the position
            // prior to current
            if (std::get<0>(*iter) == current) {
                current = std::get<1>(*iter);
                break;
            }
        }
    }
    
    /*
    int x_direction = (to.x > from.x)?1:-1;
    int y_direction = (to.y > from.y)?1:-1;

    glm::vec2 t = from;
    t.x = round(t.x + 0.4f * x_direction);
    t.y = round(t.y + 0.4f * y_direction);


    path_vector.push_back(t);

    while (t.x != to.x) {
        t.x += x_direction;
        path_vector.push_back(t);
    }

    while (t.y != to.y) {
        t.y += y_direction;
        path_vector.push_back(t);
    }
    */
   std::reverse(path_vector.begin(), path_vector.end());

    for (glm::uvec2 p : path_vector) {
        std::cout << "(" << p.x << "," << p.y << ")" << std::endl;
    }
    

    return Path(path_vector);
}

