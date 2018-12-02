#pragma once

#include "Scene.hpp"
#include "Clickable.hpp"
#include "Player.hpp"
#include "DisplayCase.hpp"
#include "FloorTile.hpp"
#include "Item.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <map>

namespace Magpie {

    struct Wall {
        Wall(Scene::Object* obj) {
            this->scene_object = obj;
        }
        Scene::Object* scene_object;
    };

    class MagpieLevel {
    public:
        // Creates a new level with the given dimensions
        MagpieLevel(uint32_t width, uint32_t length);
        ~MagpieLevel();

        // Iterates through the list of gems, painting, and interactables
        // and checks to see if the "click ray" intersects with any of the
        // objects
        void handle_click();

        // Code adopted from Grid.cpp
        // Converts a click to a floor tile position
        glm::ivec2 floor_tile_coord(glm::vec3 isect);

        // Returns true if the player is allowed to move to the
        // given position
        bool can_move_to(uint32_t current_room, uint32_t x, uint32_t y);
        bool can_move_to(uint32_t current_room, float x, float y);
        bool is_wall(float x, float y);
        bool is_wall(uint32_t x, uint32_t y);
        Wall* get_wall(float x, float y);
        Wall* get_wall(uint32_t x, uint32_t y);
        void set_wall(Wall* wall, uint32_t x, uint32_t y);
        void set_wall(Wall* wall, float x, float y);


        // Used when creating the level to get the current
        // room of a player or guard
        uint32_t get_tile_room_number(uint32_t x, uint32_t y);
        uint32_t get_tile_room_number(float x, float y);

        uint32_t get_length();
        uint32_t get_width();
        glm::uvec2 get_dimensions();
        std::vector< std::vector< bool > >* get_movement_matrix();
        void set_movement_matrix_position(uint32_t x, uint32_t y, bool can_walk);
        std::map< uint32_t, std::vector< Magpie::Painting > >* get_paintings();
        std::map< uint32_t, std::vector< Magpie::Gem > >* get_gems();
        FloorTile*** get_floor_matrix();
        std::vector< Door* >* get_doors();
        std::vector< DisplayCase* >& get_displaycases();
        

        void add_painting(uint32_t room_number, Painting painting);
        void add_gem(uint32_t room_number, Gem gem);

        
        void set_player_start_position(glm::vec3 start_position);
        glm::vec3 get_player_start_position();
        void add_guard_start_position(uint32_t room_number, uint32_t guard_number, glm::vec3 start_position);

        // Adds a position to a guards path
        void add_guard_path_position(uint32_t room_number, uint32_t guard_number, uint32_t x, uint32_t y);
        
        // Retreives a guards path, given the room number of the guard and the guards number
        std::vector< glm::vec2 > get_guard_path(uint32_t room_number, uint32_t guard_number);

        // Adds a transform to the maps of potential item positons
        void add_potential_location(std::map< uint32_t, std::vector< Scene::Transform* > >* location_map, uint32_t room_number, Scene::Transform* parent_trans);
        std::map< uint32_t, std::vector< Scene::Transform* > >* get_potential_floor_locations();
        std::map< uint32_t, std::vector< Scene::Transform* > >* get_potential_wall_locations();
        std::map< uint32_t, std::vector< Scene::Transform* > >* get_potential_pedestal_locations();
        std::map< uint32_t, std::vector< Scene::Transform* > >* get_potential_table_locations();

    protected:
 
        // Size of the entire level
        uint32_t length;    // Dimension of the level in the y-direction
        uint32_t width;     // Dimension of the level in the x-direction

        // Starting position of the player
        // NOTE:: If any of the xyz values are negative, then no
        //        starting position was specified in the level data 
        glm::vec3 player_start_position = glm::vec3(-9999.0f, -9999.0f, -9999.0f);

        // Maps room_numbers to maps of guard numbers to starting positions
        std::map< uint32_t, std::map< uint32_t, glm::vec3 > > guard_start_positions;

        // Matrix off all the tiles that the user can move to
        std::vector< std::vector< bool > > movement_matrix;

        // All the display cases placed in the game
        std::vector< DisplayCase* > displaycases;

        // How many of each item should be placed
        uint32_t gems_to_place;
        uint32_t painting_to_place;
        
        // Map Guard IDs to the positions that guards can use to navigate
        std::map< uint32_t, std::vector< glm::vec2 > > guard_paths;

        // Maps room number to the floor tiles in those rooms
        std::map< uint32_t, std::vector< FloorTile > > floor_tiles;

        // 2D array of FloorTile pointers
        FloorTile*** floor_matrix;
        // 2D array of Wall pointers
        Wall*** wall_matrix;
        // 2D array of Door Pointers
        Door*** door_matrix;

        // Probably going to use this to modify wall transparency when
        // the player moves
        std::vector< std::vector< Scene::Transform* > > walls;

        // Maps room numbers to the vectors of items placed within that room
        std::map< uint32_t, std::vector< Gem > > gems;
        std::map< uint32_t, std::vector< Painting > > paintings;
        
        // All the doors in the level
        std::vector< Door* > doors;

        // Maps room numbers to clickable objects in  the room
        std::map< uint32_t, std::vector< Clickable > >interactables;

        // Maps room numbers to transforms of floor tiles
        // where items could be dropped
        // TODO:: Create functions that give us the ability
        //        to place items at specific locations
        //        or all of the locations
        std::map< uint32_t, std::vector< Scene::Transform* > > potential_floor_locations;

        // Maps room numbers to transforms of pedestals
        // where gems could be placed
        // TODO:: Create functions that give us the ability
        //        to place gems at specific locations
        //        or all of the locations
        std::map< uint32_t, std::vector< Scene::Transform* > > potential_pedestal_locations;
        
        // Maps room numbers to transforms of walls
        // where paintings could be placed
        // TODO:: Create functions that give us the ability
        //        to place paintings at specific locations
        //        or all of the locations
        std::map< uint32_t, std::vector< Scene::Transform* > > potential_wall_locations;

        // Maps room numbers to transforms of tables/crates
        // where items could be placed
        // TODO:: Create functions that give us the ability
        //        to place items at specific locations
        //        or all of the locations
        std::map< uint32_t, std::vector< Scene::Transform* > > potential_table_locations;
        
        // Maps room numbers to maps of guard numbers to vectors of movement positions
        std::map< uint32_t, std::map< uint32_t, std::vector< glm::vec2 > > > guard_paths_by_room;
    };
}