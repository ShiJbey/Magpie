//
// Created by 何宇 on 2018/10/11.
//
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <tuple>

namespace Magpie {
    class Path {
    public:
        Path(){}
        Path(std::vector<glm::uvec2> path);
        glm::uvec2 next();
        glm::uvec2 top();
        bool isEmpty();

    private:
        std::vector<glm::uvec2> path;
        std::vector<glm::uvec2>::iterator it = path.begin();
    };

    class Navigation {
    public:
        static Navigation& getInstance() {
            static Navigation instance;
            return instance;
        }

        Navigation(Navigation const&) = delete;
        void operator=(Navigation const&) = delete;

        void set_movement_matrix(std::vector< std::vector< bool > >* matrix);
        Path findPath(glm::uvec2 start, glm::uvec2 destination);
        std::vector<glm::uvec2> get_adjacent(glm::vec2 pos);
        bool can_move_to(uint32_t x, uint32_t y);
        void print_movement_matrix();
        void reset_visited_matrix();

    private:
        Navigation() = default;
        std::vector< std::vector< bool > >* movement_matrix;
        // (visited, previous_position, distance from start)
        std::vector< std::vector< std::tuple< bool, glm::uvec2, uint32_t > > > visited_matrix;
    };
}






