#pragma once

#include "Scene.hpp"
#include "Clickable.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <map>

namespace Magpie {

    struct FloorTile : public Clickable{

    };

    struct Item {

    };

    struct Gem : public Item, public Clickable {

    };

    struct Painting: public Item, public Clickable {

    };


    struct MagpieLevel {
        MagpieLevel(uint32_t _rows, uint32_t _cols);

        // Size of the entire level
        uint32_t rows;
        uint32_t cols;

        // Matrix of walkable tiles within the game
        std::vector< std::vector< bool > > movement_matrix;
        std::vector< std::vector< bool > > interaction_map;    

        // How many of each item should be placed
        uint32_t gems_to_place;
        uint32_t painting_to_place;
        
        // Map Guard IDs to the positions that guards can use to navigate
        std::map< uint8_t, std::vector< glm::vec2 > > guard_paths;

        // Maps room number to the floor tiles in those rooms
        std::map< uint8_t, std::vector< FloorTile > >floor_tiles;

        // Maps room numbers to the vectors of items placed within that room
        std::map< uint8_t, std::vector< Gem > > gems;
        std::map< uint8_t, std::vector< Painting > >paintings;

        // Maps room numbers to clickable objects in  the room
        std::map< uint8_t, std::vector< Clickable > >interactables;

        // Iterates through the list of gems, painting, and interactables
        // and checks to see if the "click ray" intersects with any of the
        // objects
        void handle_click();

        // Code adopted from Grid.cpp
        // Converts a click to a floor tile position
        glm::uvec2 floor_tile_coord(glm::vec3 isect);

        // Returns true if the player is allowed to move to the
        // given position
        bool can_move_to(uint32_t row, uint32_t col);

        // TODO: Move the code over for removing placed items from the scene
    };
}