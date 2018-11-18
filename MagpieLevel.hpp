#pragma once

#include "Scene.hpp"
#include "Clickable.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <map>

namespace Magpie {

    class FloorTile : public Clickable{
    public:
        FloorTile( Scene::Object *object_model) {
            this->object_model = object_model;
        }

        BoundingBox* get_boundingbox() {
            // Return the existing bounding box
            if (bounding_box != nullptr) return bounding_box;
            // Create a new bounding box
            bounding_box = new BoundingBox(object_model->transform->position, glm::vec3(0.5f, 0.5f, 0.25f), glm::vec3(-0.5f, -0.5f, -0.25f));
            return bounding_box;
        }
    protected:
        Scene::Object *object_model;
    };

    class Item {
    public:
        //Scene::Object* get_object_model() { this->object_model; }
    protected:
        Scene::Object *object_model;
    };

    class Gem : public Item, public Clickable {
    public:
        BoundingBox* get_boundingbox() {
            // Return the existing bounding box
            if (bounding_box != nullptr) return bounding_box;
            // Create a new bounding box
            bounding_box = new BoundingBox(object_model->transform->position, glm::vec3(0.5f, 0.5f, 2.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
            return bounding_box;
        }
    protected:
    };

    class Painting: public Item, public Clickable {
    public:
        Painting (Scene::Object* obj_ptr) {
            this->object_model = obj_ptr;
        }

        BoundingBox* get_boundingbox() {
            // Return the existing bounding box
            if (bounding_box != nullptr) return bounding_box;
            // Create a new bounding box
            bounding_box = new BoundingBox(object_model->transform->position, glm::vec3(0.5f, 0.5f, 2.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
            return bounding_box;
        }

        void on_click() {}
    protected:
    };


    class MagpieLevel {
    public:
        MagpieLevel(uint32_t _rows, uint32_t _cols);

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

        uint32_t get_rows() { return this->rows; }
        uint32_t get_cols() { return this->cols; }
        glm::uvec2 get_dimensions() { return glm::uvec2(rows, cols); }
        std::vector< std::vector< bool > >* get_movement_matrix() { return &movement_matrix; }
        std::vector< std::vector< bool > >* get_interation_map() { return &interaction_map; }
        void add_painting(uint32_t room_number, Painting painting) {
            auto it = paintings.find(room_number);
            if (it == paintings.end()) {
                paintings.insert({room_number, {painting}}); 
            }
            else{
                paintings[room_number].push_back(painting);
            }
        }

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
        std::map< uint32_t, std::vector< glm::vec2 > > guard_paths;

        // Maps room number to the floor tiles in those rooms
        std::map< uint32_t, std::vector< FloorTile > >floor_tiles;

        // Maps room numbers to the vectors of items placed within that room
        std::map< uint32_t, std::vector< Gem > > gems;
        std::map< uint32_t, std::vector< Painting > >paintings;

        // Maps room numbers to clickable objects in  the room
        std::map< uint32_t, std::vector< Clickable > >interactables;
    };
}