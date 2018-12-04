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

        Navigation();
        Navigation(std::vector< std::vector< bool > >* level_movement_matrix);

        static Navigation& getInstance() {
            static Navigation instance;
            return instance;
        }

        void init(std::vector< std::vector< bool > >* level_movement_matrix);
        void set_movement_matrix(std::vector< std::vector< bool > >* level_movement_matrix);
        Path findPath(glm::vec2 start, glm::vec2 destination);
        std::vector<glm::vec2> get_adjacent(glm::vec2 pos);
        bool can_move_to(float x, float y);
        void print_movement_matrix();
        void reset_visited_matrix();

    private:
        

        // Pointer to the movement matrix managed by a
        // Magpie Level
        std::vector< std::vector< bool > >* level_movement_matrix;

        // (visited, previous_position, distance from start)
        std::vector< std::vector< std::tuple< bool, glm::vec2, float > > > visited_matrix;
    };
}






