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

        // NOTE: This is probably going to move to the PlayerModel class
        glm::vec2 player_position;

        
        uint32_t gems_to_place;
        uint32_t painting_to_place;
        // References to places where we can place gems/paintings
        std::vector< Scene::Transform* > potential_pedestal_locations;
        std::vector< Scene::Transform* > potential_wall_locations;

        std::map< uint8_t, std::vector< glm::vec2 > > guard_paths;

        std::vector< glm::vec2 > moveable_tiles;

        std::vector< Scene::Object* > placed_items;

        std::vector<Entity*> entities;
        Entity* player;

        Scene::Object* remove_placed_item(uint32_t row, uint32_t col) {
            Scene::Object* removed_item = nullptr;
            uint32_t index_to_remove = 0;
            for (uint32_t i = 0; i < placed_items.size(); i++) {
                 Scene::Object* item = placed_items[i];
                if (row == (uint32_t)item->transform->position.x && col == (uint32_t)item->transform->position.y) {
                    removed_item = item;
                    index_to_remove = i;
                    break;
                }
            }
            if (removed_item != nullptr) {
                placed_items.erase(placed_items.begin() + index_to_remove);
            }
            return removed_item;
        }

    };

    extern MagpieGame game;
}