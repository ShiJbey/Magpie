//
// Created by 何宇 on 2018/10/11.
//

#include "Navigation.h"
#include <iostream>
#include <deque>
#include <map>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>
#include <iterator>

/**
 *  The Implementations of class Path.
 */

Magpie::Path::Path(std::vector<glm::uvec2> path) {
    this->path = std::move(path);
    it = this->path.begin();
}

glm::uvec2 Magpie::Path::next() {
    //std::cout << "NEXT:" << it->x << ", " << it->y << std::endl;
    if (it == path.end()) return glm::uvec2(0, 0);
    return *(it++);
}

bool Magpie::Path::isEmpty() {
    return it == path.end();
}

glm::uvec2 Magpie::Path::top() {
    return *path.begin();
}

bool Magpie::Navigation::can_move_to(uint32_t x, uint32_t y) {
    if (this->movement_matrix == nullptr) {
        return false;
    } 
    else if (x >= (*movement_matrix).size() || y >= (*movement_matrix)[x].size()) {
        return false;
    }

    return (*movement_matrix)[x][y];
}

void Magpie::Navigation::print_movement_matrix() {
    for (uint32_t y = (uint32_t)(*movement_matrix)[0].size() - 1; y != -1U; y--) {
        for (uint32_t x = 0; x < (*movement_matrix).size(); x++) {
            if ((*movement_matrix)[x][y]) {
                std::cout << "[ ]";
            } else {
                std::cout << "[x]";
            }
        }
        std::cout << std::endl;
    }
};

void Magpie::Navigation::reset_visited_matrix() {
    for (uint32_t x = 0; x < visited_matrix.size(); x++) {
        for (uint32_t y = 0; y < visited_matrix[x].size(); y++) {
            visited_matrix[x][y] = std::make_tuple(false, glm::uvec2(x, y), -1U);
        }
    }
};

std::vector<glm::uvec2> Magpie::Navigation::get_adjacent(glm::vec2 pos) {
    std::vector<glm::uvec2> adjacent_tiles;
    
    if (pos.x > 0) {

        // Check position to the left
        if (can_move_to((uint32_t)pos.x - 1, (uint32_t)pos.y)) {
            adjacent_tiles.push_back(glm::uvec2((uint32_t)pos.x - 1, (uint32_t)pos.y));
        }
        
        // Check position bottom left
        if (pos.y > 0) {
            if (can_move_to((uint32_t)pos.x - 1, (uint32_t)pos.y - 1)) {
                adjacent_tiles.push_back(glm::uvec2((uint32_t)pos.x - 1, (uint32_t)pos.y - 1));
            }
        }

        // Check upper left
        if (pos.y < -1U) {
            if (can_move_to((uint32_t)pos.x - 1, (uint32_t)pos.y + 1)) {
                adjacent_tiles.push_back(glm::uvec2((uint32_t)pos.x - 1, (uint32_t)pos.y + 1));
            }
        }

    }

    if (pos.x < -1U) {
        // Check to the right
        if (can_move_to((uint32_t)pos.x + 1, (uint32_t)pos.y)) {
            adjacent_tiles.push_back(glm::uvec2((uint32_t)pos.x + 1, (uint32_t)pos.y));
        }

        // Check position bottom right
        if (pos.y > 0) {
            if (can_move_to((uint32_t)pos.x + 1, (uint32_t)pos.y - 1)) {
                adjacent_tiles.push_back(glm::uvec2((uint32_t)pos.x + 1, (uint32_t)pos.y - 1));
            }
        }

        // Check upper right
        if (pos.y < -1U) {
            if (can_move_to((uint32_t)pos.x + 1, (uint32_t)pos.y + 1)) {
                adjacent_tiles.push_back(glm::uvec2((uint32_t)pos.x + 1, (uint32_t)pos.y + 1));
            }
        }
    }


    if (pos.y < -1U) {
        if (can_move_to((uint32_t)pos.x, (uint32_t)pos.y + 1)) {
            adjacent_tiles.push_back(glm::uvec2((uint32_t)pos.x, (uint32_t)pos.y + 1));
        }
    }
    
    if (pos.y > 0) {
        if (can_move_to((uint32_t)pos.x, (uint32_t)pos.y - 1)) {
            adjacent_tiles.push_back(glm::uvec2((uint32_t)pos.x, (uint32_t)pos.y - 1));
        }
    }
    
    
    // From https://en.cppreference.com/w/cpp/algorithm/random_shuffle
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(adjacent_tiles.begin(), adjacent_tiles.end(), g);
    return adjacent_tiles;
};

/**
 *  The Implementations of class Navigation.
 */

void Magpie::Navigation::set_movement_matrix(std::vector< std::vector< bool > >* matrix) {
    this->movement_matrix = matrix;
    this->visited_matrix.clear();
    if (movement_matrix != nullptr) {
        for (uint32_t x = 0; x < (*movement_matrix).size(); x++) {
            std::vector< std::tuple< bool, glm::uvec2, uint32_t > > col;
            for (uint32_t y = 0; y < (*movement_matrix)[x].size(); y++) {
                col.push_back(std::make_tuple(false, glm::uvec2(x, y), -1U));
            }
            visited_matrix.push_back(col);
        }

        print_movement_matrix();
    }
    else {
        std::cerr << "WARNING: Navigation has been pass a null movement matrix." << std::endl;
    }
}

Magpie::Path Magpie::Navigation::findPath(glm::uvec2 start, glm::uvec2 destination) {
    reset_visited_matrix();
    
    std::cout << "Current Magpie Position: (x: " <<  start.x << " , y: " << start.y << " )" << std::endl;
    std::vector<glm::uvec2> path_vector;

    // Implement BFS for path finding
    // https://www.redblobgames.com/pathfinding/tower-defense/
    
    // Unexplored grid positions
    std::deque< glm::uvec2 > frontier;
    frontier.push_back(glm::uvec2(start));

    visited_matrix[start.x][start.y] = std::make_tuple(true, glm::uvec2(start), 0);

    // Perform BFS
    while( !frontier.empty() ) {

        glm::uvec2 current = frontier.front();

        frontier.pop_front();

        std::vector<glm::uvec2> adjacent_tiles = get_adjacent(current);

        for (auto it = adjacent_tiles.begin(); it != adjacent_tiles.end(); it++) {
            
            bool position_visited = std::get<0>(visited_matrix[it->x][it->y]);
            uint32_t distance_from_start =  std::get<2>(visited_matrix[current.x][current.y]) + 1;

            if (distance_from_start < std::get<2>(visited_matrix[it->x][it->y])) {
                visited_matrix[it->x][it->y] = std::make_tuple(true, current, distance_from_start);
            }

            if (!position_visited) {
                frontier.push_back(*it);
            }

            if (*it == destination) {
                break;
            }
        }
    }

    // Reverse iterate to get a path
    glm::uvec2 current = destination;
    while (current != glm::uvec2(start)) {
        path_vector.push_back(current);
        current = std::get<1>(visited_matrix[current.x][current.y]);
    }
    
    //path_vector.pop_back();
    std::reverse(path_vector.begin(), path_vector.end());
    for (glm::uvec2 p : path_vector) {
        std::cout << "(" << p.x << "," << p.y << ")" << std::endl;
    }
    

    return Path(path_vector);
}

