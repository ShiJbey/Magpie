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

// NOTE:: When indexing into any of the level grids, use y then x
//      (e.g level_navigation_grid[y][x])
bool Magpie::Navigation::can_move_to(uint32_t x, uint32_t y) {
    if (this->level_navigation_grid == nullptr) {
        return false;
    }

    else if (y >= level_navigation_grid->size() || x >= (*level_navigation_grid)[y].size()) {
        return false;
    }

    return (*level_navigation_grid)[(uint32_t)y][(uint32_t)x];
}

void Magpie::Navigation::print_navigation_grid() {
    for (uint32_t y = (uint32_t)(*level_navigation_grid).size() - 1; y != -1U; y--) {
        for (uint32_t x = 0; x < (*level_navigation_grid)[y].size(); x++) {
            if ((*level_navigation_grid)[y][x]) {
                std::cout << "[ ]";
            } else {
                std::cout << "[x]";
            }
        }
        std::cout << std::endl;
    }
};

void Magpie::Navigation::reset_visited_grid() {
    for (uint32_t y = 0; y < visited_grid.size(); y++) {
        for (uint32_t x = 0; x < visited_grid[y].size(); x++) {
            // NOTE:: When saving position vectors, we save as glm:vec2(x,y)
            //      instead of the reverse which is used when inddexing into the level
            visited_grid[y][x] = std::make_tuple(false, glm::vec2((float)x, (float)y), 100000.0f);
        }
    }
};

std::vector<glm::vec2> Magpie::Navigation::get_adjacent(glm::vec2 pos) {
    std::vector<glm::vec2> adjacent_tiles;

    // Check position to the left
    if (can_move_to((uint32_t)pos.x - 1, (uint32_t)pos.y)) {
        adjacent_tiles.push_back(glm::vec2(pos.x - 1, pos.y));
    }

    // Check to the right
    if (can_move_to((uint32_t)pos.x + 1, (uint32_t)pos.y)) {
        adjacent_tiles.push_back(glm::vec2(pos.x + 1, pos.y));
    }

    // Check above
    if (can_move_to((uint32_t)pos.x, (uint32_t)pos.y + 1)) {
        adjacent_tiles.push_back(glm::vec2(pos.x, pos.y + 1));
    }

    // Check below
    if (can_move_to((uint32_t)pos.x, (uint32_t)pos.y - 1)) {
        adjacent_tiles.push_back(glm::vec2(pos.x, pos.y - 1));
    }

    // From https://en.cppreference.com/w/cpp/algorithm/random_shuffle
    // std::random_device rd;
    // std::mt19937 g(rd());
    // std::shuffle(adjacent_tiles.begin(), adjacent_tiles.end(), g);
    return adjacent_tiles;
};

void Magpie::Navigation::set_navigation_grid(std::vector< std::vector< bool > >* level_navigation_grid) {
    this->level_navigation_grid = level_navigation_grid;
    this->visited_grid.clear();
    if (level_navigation_grid != nullptr) {
        for (uint32_t y = 0; y < level_navigation_grid->size(); y++) {
            std::vector< std::tuple< bool, glm::vec2, float > > row;
            for (uint32_t x = 0; x < (*level_navigation_grid)[y].size(); x++) {
                row.push_back(std::make_tuple(false, glm::vec2((float)x, (float)y), 100000.0f));
            }
            visited_grid.push_back(row);
        }
    }
    else {
        std::cerr << "WARNING: Navigation has been passed a null navigation grid." << std::endl;
    }
};

// Implement BFS for path finding
// https://www.redblobgames.com/pathfinding/tower-defense/
Magpie::Path Magpie::Navigation::findPath(glm::vec2 start, glm::vec2 destination) {

    reset_visited_grid();

    // Unexplored grid positions
    std::deque< glm::vec2 > frontier;

    float start_x = glm::round(start.x);
    float start_y = glm::round(start.y);

    frontier.emplace_back(glm::vec2(start_x, start_y));

    visited_grid[(uint32_t)start_y][(uint32_t)start_x] = std::make_tuple(true, glm::vec2(-1.0f, -1.0f), 0.0f);

    bool destination_reached = false;

    // Perform BFS
    while( !frontier.empty() ) {

        glm::vec2 current = frontier.front();

        frontier.pop_front();

        std::vector<glm::vec2> adjacent_tiles = get_adjacent(current);

        for (auto it = adjacent_tiles.begin(); it != adjacent_tiles.end(); it++) {

            bool position_visited = std::get<0>(visited_grid[(uint32_t)it->y][(uint32_t)it->x]);
            float distance_from_start =  std::get<2>(visited_grid[(uint32_t)current.y][(uint32_t)current.x]) + glm::length(*it - destination);

            if (distance_from_start < std::get<2>(visited_grid[(uint32_t)it->y][(uint32_t)it->x])) {
                visited_grid[(uint32_t)it->y][(uint32_t)it->x] = std::make_tuple(true, current, distance_from_start);
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

    // Reverse iterate through visited grid to get a path
    glm::vec2 current = destination;
    while (current != glm::vec2(-1, -1)) {
        path_vector.push_back(current);
        current = std::get<1>(visited_grid[(uint32_t)current.y][(uint32_t)current.x]);
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



void Magpie::Navigation::init(std::vector< std::vector< bool > >* level_navigation_grid) {
    this->level_navigation_grid = level_navigation_grid;
};

Magpie::Navigation::Navigation() : Navigation::Navigation(nullptr) {
    // Do Nothing
};

Magpie::Navigation::Navigation(std::vector< std::vector< bool > >* level_navigation_grid) {
    this->level_navigation_grid = level_navigation_grid;
};

