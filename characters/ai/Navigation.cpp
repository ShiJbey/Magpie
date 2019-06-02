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

//////////////////////////////////////////////
//                   PATH                   //
//////////////////////////////////////////////

Magpie::Path::Path(std::vector<glm::vec2> path) {
    this->path = std::move(path);
    it = this->path.begin();
}

glm::vec2 Magpie::Path::next() {
    if (it == path.end()) {
        std::cerr << "ERROR::Path.next():: Complete path giving additional positions" << std::endl;
        return glm::vec2(0, 0);
    }
    return *(it++);
}

bool Magpie::Path::isEmpty() {
    return it == path.end();
}

glm::vec2 Magpie::Path::top() {
    return *path.begin();
}

//////////////////////////////////////////////
//               NAVIGATION                 //
//////////////////////////////////////////////


bool Magpie::Navigation::can_move_to(float x, float y) {
    if (this->level_movement_matrix == nullptr) {
        return false;
    }

    else if ((uint32_t)x >= level_movement_matrix->size() || (uint32_t)y >= (*level_movement_matrix)[(uint32_t)x].size()) {
        return false;
    }

    return (*level_movement_matrix)[(uint32_t)x][(uint32_t)y];
}

void Magpie::Navigation::print_movement_matrix() {
    for (uint32_t y = (uint32_t)(*level_movement_matrix)[0].size() - 1; y != -1U; y--) {
        for (uint32_t x = 0; x < (*level_movement_matrix).size(); x++) {
            if ((*level_movement_matrix)[x][y]) {
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
            visited_matrix[x][y] = std::make_tuple(false, glm::vec2((float)x, (float)y), 100000.0f);
        }
    }
};

std::vector<glm::vec2> Magpie::Navigation::get_adjacent(glm::vec2 pos) {
    std::vector<glm::vec2> adjacent_tiles;

    // Check position to the left
    if (can_move_to(pos.x - 1, pos.y)) {
        adjacent_tiles.push_back(glm::vec2(pos.x - 1, pos.y));
    }

    // Check to the right
    if (can_move_to(pos.x + 1, pos.y)) {
        adjacent_tiles.push_back(glm::vec2(pos.x + 1, pos.y));
    }

    // Check above
    if (can_move_to(pos.x, pos.y + 1)) {
        adjacent_tiles.push_back(glm::vec2(pos.x, pos.y + 1));
    }

    // Check below
    if (can_move_to(pos.x, pos.y - 1)) {
        adjacent_tiles.push_back(glm::vec2(pos.x, pos.y - 1));
    }
    
    // From https://en.cppreference.com/w/cpp/algorithm/random_shuffle
    // std::random_device rd;
    // std::mt19937 g(rd());
    // std::shuffle(adjacent_tiles.begin(), adjacent_tiles.end(), g);
    return adjacent_tiles;
};

void Magpie::Navigation::set_movement_matrix(std::vector< std::vector< bool > >* level_movement_matrix) {
    this->level_movement_matrix = level_movement_matrix;
    this->visited_matrix.clear();
    if (level_movement_matrix != nullptr) {
        for (uint32_t x = 0; x < level_movement_matrix->size(); x++) {
            std::vector< std::tuple< bool, glm::vec2, float > > col;
            for (uint32_t y = 0; y < (*level_movement_matrix)[x].size(); y++) {
                col.push_back(std::make_tuple(false, glm::vec2((float)x, (float)y), 100000.0f));
            }
            visited_matrix.push_back(col);
        }
    }
    else {
        std::cerr << "WARNING: Navigation has been pass a null movement matrix." << std::endl;
    }
};

// Implement BFS for path finding
// https://www.redblobgames.com/pathfinding/tower-defense/
Magpie::Path Magpie::Navigation::findPath(glm::vec2 start, glm::vec2 destination) {
    
    reset_visited_matrix();

    // Unexplored grid positions
    std::deque< glm::vec2 > frontier;

    float start_x = glm::round(start.x);
    float start_y = glm::round(start.y);

    //uint64_t start_x = trunc(start.x);
    //uint64_t start_y = trunc(start.y);
    
    //double decimal_x = start.x - trunc(start.x);
    //double decimal_y = start.y - trunc(start.y);

    frontier.emplace_back(glm::uvec2(start_x, start_y));

    visited_matrix[(uint32_t)start_x][(uint32_t)start_y] = std::make_tuple(true, glm::vec2(-1.0f, -1.0f), 0.0f);

    bool destination_reached = false;

    // Perform BFS
    while( !frontier.empty() ) {

        glm::vec2 current = frontier.front();

        frontier.pop_front();

        std::vector<glm::vec2> adjacent_tiles = get_adjacent(current);

        for (auto it = adjacent_tiles.begin(); it != adjacent_tiles.end(); it++) {

            bool position_visited = std::get<0>(visited_matrix[(uint32_t)it->x][(uint32_t)it->y]);
            float distance_from_start =  std::get<2>(visited_matrix[(uint32_t)current.x][(uint32_t)current.y]) + glm::length(*it - destination);

            if (distance_from_start < std::get<2>(visited_matrix[(uint32_t)it->x][(uint32_t)it->y])) {
                visited_matrix[(uint32_t)it->x][(uint32_t)it->y] = std::make_tuple(true, current, distance_from_start);
            }

            if (!position_visited) {
                frontier.push_back(*it);
            }

            if (*it == destination) {
                destination_reached = true;
                break;
            }
        }
    }


    std::vector<glm::vec2> path_vector;

    // We return an empty path vector if the destination could not be reached
    if (!destination_reached) {
        return path_vector;
    }

    // Reverse iterate through visited matrix to get a path
    glm::vec2 current = destination;
    while (current != glm::vec2(-1, -1)) {
        path_vector.push_back(current);
        current = std::get<1>(visited_matrix[(uint32_t)current.x][(uint32_t)current.y]);
    }
    // Reverse the path to get it in the correct order
    std::reverse(path_vector.begin(), path_vector.end());
    
    #ifdef NAVIGATION_DEBUG_VERBOSE
    std::cout << "DEBUG::Navigation.findPath():: Generated Path" << std::endl;
    for (glm::vec2 p : path_vector) {
        std::cout << "\t(" << p.x << "," << p.y << ")" << std::endl;
    }
    #endif
    

    return Path(path_vector);
};



void Magpie::Navigation::init(std::vector< std::vector< bool > >* level_movement_matrix) {
    this->level_movement_matrix = level_movement_matrix;
};

Magpie::Navigation::Navigation() : Navigation::Navigation(nullptr) {
    // Do Nothing
};

Magpie::Navigation::Navigation(std::vector< std::vector< bool > >* level_movement_matrix) {
    this->level_movement_matrix = level_movement_matrix;
};

