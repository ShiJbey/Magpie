#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include "Scene.hpp"
#include "Entity.h"

#include <vector>
#include <map>

namespace Magpie {

    // Using this class to organize all important game logic
    struct MagpieGame {

        std::vector< Scene::Transform* > potential_pedestal_locations;
        std::vector< Scene::Transform* > potential_wall_locations;

        std::vector< glm::vec2 > moveable_tiles;

        std::vector< Scene::Transform* > placed_items;

        std::vector<Entity*> entities;
        Entity* player;

    };

    extern MagpieGame game;
}