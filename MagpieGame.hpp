#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include "Scene.hpp"

#include <vector>
#include <map>

namespace Magpie {

    // Using this class to organize all important game logic
    struct MagpieGame {

        // NOTE: This is probably going to move to the PlayerModel class
        glm::vec2 player_position;

        
        uint32_t gems_to_place;
        uint32_t painting_to_place;
        // References to places where we can place gems/paintings
        std::vector< Scene::Transform* > potential_pedestal_locations;
        std::vector< Scene::Transform* > potential_wall_locations;

        std::map< uint8_t, std::vector< glm::vec2 > > guard_paths;

        std::vector< glm::vec2 > moveable_tiles;

        std::vector< Scene::Transform* > placed_items;

    };

}