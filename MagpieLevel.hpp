#pragma once

#include "Scene.hpp"
#include "Clickable.hpp"
#include "Player.hpp"
#include "FloorTile.hpp"
#include "Item.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <map>

namespace Magpie {

    class MagpieLevel {
    public:
        // Creates a new level with the given dimensions
        MagpieLevel(uint32_t width, uint32_t length);

        // Iterates through the list of gems, painting, and interactables
        // and checks to see if the "click ray" intersects with any of the
        // objects
        void handle_click();

        // Code adopted from Grid.cpp
        // Converts a click to a floor tile position
        glm::uvec2 floor_tile_coord(glm::vec3 isect);

        // Returns true if the player is allowed to move to the
        // given position
        bool can_move_to(uint32_t x, uint32_t y);

        uint32_t get_length();
        uint32_t get_width();
        glm::uvec2 get_dimensions();
        std::vector< std::vector< bool > >* get_movement_matrix();
        void set_movement_matrix_position(uint32_t x, uint32_t y, bool can_walk);
        std::map< uint32_t, std::vector< Magpie::Painting > >* get_paintings();

        void add_painting(uint32_t room_number, Painting painting);

        // Adds a position to a guards path
        void add_guard_path_position(uint32_t room_number, uint32_t guard_number, uint32_t x, uint32_t y);
    protected:

        // Size of the entire level
        uint32_t length;    // Dimension of the level in the y-direction
        uint32_t width;     // Dimension of the level in the x-direction

        // Matrix off all the tiles that the user can move to
        std::vector< std::vector< bool > > movement_matrix; 

        // How many of each item should be placed
        uint32_t gems_to_place;
        uint32_t painting_to_place;
        
        // Map Guard IDs to the positions that guards can use to navigate
        std::map< uint32_t, std::vector< glm::vec2 > > guard_paths;

        // Maps room number to the floor tiles in those rooms
        std::map< uint32_t, std::vector< FloorTile > >floor_tiles;

        // Maps room numbers to the vectors of items placed within that room
        std::map< uint32_t, std::vector< Gem > > gems;
        std::map< uint32_t, std::vector< Painting > > paintings;

        // Maps room numbers to clickable objects in  the room
        std::map< uint32_t, std::vector< Clickable > >interactables;

        // Maps room numbers to maps of guard numbers to vectors of movement positions
        std::map< uint32_t, std::map< uint32_t, std::vector< glm::vec2 > > > guard_paths_by_room;
    };
}