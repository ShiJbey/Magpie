#pragma once

#include "objects/DisplayCase.hpp"
#include "objects/FloorTile.hpp"
#include "objects/Wall.hpp"
#include "objects/Item.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <map>

namespace Magpie {

    class MagpieLevel {
    public:
        // Contrsucts a new level with a given width and length
        MagpieLevel(uint32_t width, uint32_t length);
        ~MagpieLevel();

        // Converts a click to a floor tile position
        glm::ivec2 floor_tile_coord(glm::vec3 isect);

        // Checks that a given position is within the level bounds
        bool is_within_bounds(uint32_t x, uint32_t y);
        bool is_within_bounds(float x, float y);

        // Check id a player in a given room can move to a given position
        bool can_move_to(uint32_t current_room, uint32_t x, uint32_t y);
        bool can_move_to(uint32_t current_room, float x, float y);

        // Returns true if the object at this position is a wall
        bool is_wall(float x, float y);
        bool is_wall(uint32_t x, uint32_t y);

        // Returns pointer to wall at given position, or nullptr if no wall
        Wall* get_wall(float x, float y);
        Wall* get_wall(uint32_t x, uint32_t y);

        // Sets a wall pointer at the given x and y position
        void set_wall(Wall* wall, uint32_t x, uint32_t y);
        void set_wall(Wall* wall, float x, float y);

        // Returns the room number of the FloorTile at the given position
        uint32_t get_tile_room_number(uint32_t x, uint32_t y);
        uint32_t get_tile_room_number(float x, float y);

        // Get different dimensions
        uint32_t get_length() { return this->length; }
        uint32_t get_width() { return this->width; }
        glm::uvec2 get_dimensions() { return glm::uvec2(this->width, this->length); }

        // Get the various environment grids
        std::vector< std::vector< bool > >* get_nav_grid() { return &(this->nav_grid); }
        std::vector< std::vector< Wall* > >* get_wall_grid() { return &(this->wall_grid); }
        std::vector< std::vector< FloorTile* > >* get_floor_grid() { return &(this->floor_grid); }
        std::vector< Door* >* get_doors() { return &(this->doors); };

        // Set values for various grids
        FloorTile* get_floor_tile(uint32_t x, uint32_t y) { return floor_grid[y][x]; }
        void set_floor_tile(FloorTile* tile, uint32_t x, uint32_t y) { floor_grid[y][x] = tile; }

        // Get collectables
        std::vector< DisplayCase* >* get_displaycases() { return &(this->displaycases); }
        std::map< uint32_t, std::vector< Magpie::Painting* > >* get_paintings() { return &(this->paintings); }
        std::map< uint32_t, std::vector< Magpie::Gem* > >* get_gems() { return &(this->gems); }

        // Treat checking for guards
        std::vector< std::vector< DogTreat* > >* get_treat_grid() { return &(this->treat_grid); }
        DogTreat* get_treat(glm::vec2 location);
        void set_treat(glm::vec2 location, DogTreat* treat);

        // Modify the starting position of the Player
        glm::vec3 get_player_start_position() { return this->player_start_position; }
        void set_player_start_position(glm::vec3 start_position);

        // Retreives gard path information
        std::map< uint32_t, std::map< uint32_t, std::pair<glm::vec3, uint32_t > > >* get_guard_start_positions();
        std::vector< glm::vec2 > get_guard_path(uint32_t room_number, uint32_t guard_number);

        // Sets a value in the nav_grid indicating if Game Characters can walk on it
        void set_nav_grid_position(uint32_t x, uint32_t y, bool can_walk);

        // Getters and Setters for Key Items
        KeyCard* get_green_card() { return this->green_card; }
        KeyCard* get_pink_card() { return this->pink_card; }
        KeyCard* get_master_key() { return this->master_key; }
        DogTreat* get_dog_treat() { return this->dogTreatPickUp; }
        CardboardBox* get_cardboard_box() { return this->cardboard_box; }
        BackExit* get_back_exit() { return this->back_exit; }

        void set_green_card(KeyCard* card) { this->green_card = card; }
        void set_pink_card(KeyCard* card) { this->pink_card = card; }
        void set_master_key(KeyCard* card) { this->master_key = card; }
        void set_dog_treat(DogTreat* treat) { this->dogTreatPickUp = treat; }
        void set_cardboard_box(CardboardBox* box) { this->cardboard_box = box; }
        void set_back_exit(BackExit* exit) { this->back_exit = exit; }

        // Other functions
        void add_displaycase(DisplayCase* displaycase);
        void add_painting(uint32_t room_number, Painting* painting);
        void add_gem(uint32_t room_number, Gem* gem);
        void add_guard_start_position(uint32_t room_number, uint32_t guard_number, glm::vec3 start_position, uint32_t dir);
        void add_guard_path_position(uint32_t room_number, uint32_t guard_number, uint32_t x, uint32_t y);

    protected:

        // Size of the entire level
        uint32_t length;    // Dimension of the level in the y-direction
        uint32_t width;     // Dimension of the level in the x-direction

        // Starting position of the player
        // NOTE:: If any of the xyz values are negative, then no
        //        starting position was specified in the level data
        glm::vec3 player_start_position = glm::vec3(-9999.0f, -9999.0f, -9999.0f);

        // Maps room_numbers to maps of guard numbers to starting positions
        std::map< uint32_t, std::map< uint32_t, std::pair<glm::vec3, uint32_t > > > guard_start_positions;

        // Map Guard IDs to the vector of positions making up this guard's path
        std::map< uint32_t, std::vector< glm::vec2 > > guard_paths;

        // Maps a room number to a vector the floor tiles in that room
        std::map< uint32_t, std::vector< FloorTile* > > floor_tiles;

        // 2D vector of FloorTile pointers
        std::vector< std::vector< FloorTile* > > floor_grid;
        // 2D vector of Wall pointers
        std::vector< std::vector< Wall* > > wall_grid;
        // 2D vector of Door Pointers
        std::vector< std::vector< Door* > > door_grid;
        // 2D vector of bools indicating where Game Characters may move
        std::vector< std::vector< bool > > nav_grid;
        // 2D vector of DogTreats in the game
        std::vector< std::vector< DogTreat* > > treat_grid;
        // All the doors in the level
        std::vector< Door* > doors;

        // Maps room numbers to the vectors of items placed within that room
        std::map< uint32_t, std::vector< Gem* > > gems;
        std::map< uint32_t, std::vector< Painting* > > paintings;
        // All the display cases placed in the game
        std::vector< DisplayCase* > displaycases;

        // Maps room numbers to maps of guard numbers to vectors of movement positions
        std::map< uint32_t, std::map< uint32_t, std::vector< glm::vec2 > > > guard_paths_by_room;

        // Key Items placed in level
        KeyCard *green_card = nullptr;
        KeyCard *pink_card = nullptr;
        KeyCard *master_key = nullptr;
        DogTreat *dogTreatPickUp = nullptr;
        CardboardBox *cardboard_box = nullptr;
        BackExit *back_exit = nullptr;
    };
}