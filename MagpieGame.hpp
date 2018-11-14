#pragma once

#include "Player.hpp"
#include "Guard.hpp"
#include "MagpieLevel.hpp"
#include "Scene.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <deque>


namespace Magpie {
    // Using this class to organize all important game logic
    struct MagpieGame {

        // NOTE: This is probably going to move to the PlayerModel class
        glm::vec2 player_position;

        Player player;
        std::vector< Guard > guards;

        uint32_t gems_to_place;
        uint32_t painting_to_place;
        // References to places where we can place gems/paintings
        std::vector< Scene::Transform* > potential_pedestal_locations;
        std::vector< Scene::Transform* > potential_wall_locations;


        

        // Maintain references to all the floor tiles
        // NOTE:: We may use this for highlighting the floor tiles during movement
        std::vector< Scene::Object* > floor_tiles;

        // All the objects that have been placed for the player to obtain
        std::vector< Scene::Object* > placed_items;

        Scene::Object* remove_placed_item(uint32_t x, uint32_t y);

        MagpieLevel *current_level;

        
        

    };

    extern MagpieGame game;
}