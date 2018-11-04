//
// Created by 何宇 on 2018/10/11.
//
#pragma once

#include "MagpieLevel.hpp"

#include <vector>

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

        void loadLevel(MagpieLevel* map);
        Path findPath(glm::vec2 from, glm::uvec2 to);

    private:
        Navigation() = default;
        MagpieLevel* level;
    };
}






