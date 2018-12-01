//
// Created by 何宇 on 2018/10/11.
//
#pragma once

#include "FloorTile.hpp"
#include "Door.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <tuple>

namespace Magpie {
    class Path {
    public:
        Path(){}
        Path(std::vector<glm::vec2> path);
        glm::vec2 next();
        glm::vec2 top();
        bool isEmpty();
        std::vector<glm::vec2> get_path() { return this->path; };
        void set_path(std::vector<glm::vec2> p) {
            this->path = p;
            it = this->path.begin();
        };

    private:
        std::vector<glm::vec2> path;
        std::vector<glm::vec2>::iterator it = path.begin();
    };

    class Navigation {
    public:
        static Navigation& getInstance() {
            static Navigation instance;
            return instance;
        }

        Navigation(Navigation const&) = delete;
        void operator=(Navigation const&) = delete;

        void init(FloorTile*** floor_tiles, Door*** doors, uint32_t level_width, uint32_t level_height);
        void set_movement_matrix(std::vector< std::vector< bool > >* matrix);
        Path findPath(glm::vec2 start, glm::vec2 destination);
        std::vector<glm::vec2> get_adjacent(glm::vec2 pos);
        bool can_move_to(float x, float y);
        void print_movement_matrix();
        void reset_visited_matrix();

    private:
        Navigation() = default;

        std::vector< std::vector< bool > >* movement_matrix;

        uint32_t level_width;
        uint32_t level_height;
        FloorTile*** floor_tiles;
        Door*** doors;

        // (visited, previous_position, distance from start)
        std::vector< std::vector< std::tuple< bool, glm::vec2, float > > > visited_matrix;
    };
}






