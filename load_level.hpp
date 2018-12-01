#pragma once

#include "MagpieGame.hpp"
#include "Scene.hpp"
#include "MeshBuffer.hpp"


#include <map>
#include <string>
#include <vector>
#include <unordered_map>


namespace Magpie {
    // Pixels are read into this
    struct PixelData {
        
        uint8_t red_channel_data;
        uint8_t green_channel_data;
        uint8_t blue_channel_data;

        PixelData();
        PixelData(uint8_t red_channel_data_, uint8_t green_channel_data_, uint8_t blue_channel_data_);

        std::string to_string();

        // Get the ID of the mesh we are supposed to place
        uint8_t get_mesh_id();
        // What modifiers are we placing on the mesh's appearance
        uint8_t get_mesh_customization();
        // Room number this tile belongs to
        uint8_t get_room_number();
        // Can we place either a gem or a painting at this tile
        bool is_item_location();
        // Get the number id of the guard who walks using this position
        // in its path
        uint8_t guard_path_number();

        // NOTE: The following are for implementing high level mechanics
        // Does the mesh at this position have a special identifier
        uint8_t get_object_id();
        uint8_t get_group_id();
        // What interaction lambda function is associated with this game object
        uint8_t get_interaction_func_id();
        // Does the interaction function need to set or check a global flag
        uint8_t get_interaction_flag();

        // Returns true if this is a wall, corner, or door
        bool is_wall_corner_door();

        bool is_player_start_position();
        bool is_guard_start_position();
        
        // Checks if there are objects that are considered walls
        // to the left and right of the given pixel position
        // Walls, doors, and corners all count was "walls"
        static bool walls_to_left_and_right(std::vector< PixelData > level_pixels, uint32_t level_width, uint32_t x, uint32_t y);

        // Checks if there are objects that are considered walls
        // to the top and bottom of the given pixel position
        // Walls, doors, and corners all count was "walls"
        static bool walls_to_top_and_bottom(std::vector< PixelData > level_pixels, uint32_t level_width, uint32_t x, uint32_t y);

        

        // MASK VALUES
        static uint8_t MESH_MASK;
        static uint8_t MESH_CUSTOMIZATION_MASK;
        static uint8_t ROOM_NUMBER_MASK;
        static uint8_t GUARD_PATH_MASK;
        static uint8_t ITEM_LOCATION_MASK;
        static uint8_t GROUP_ID_MASK;
        static uint8_t OBJECT_ID_MASK;

        // OFFSET VALUES
        static uint8_t MESH_OFFSET;
        static uint8_t MESH_CUSTOMIZATION_OFFSET;
        static uint8_t ROOM_NUMBER_OFFSET;
        static uint8_t GUARD_PATH_OFFSET;
        static uint8_t ITEM_LOCATION_OFFSET;
        static uint8_t GROUP_ID_OFFSET;
        static uint8_t OBJECT_ID_OFFSET;

    };

    // This is for statically loading level pixel data
    struct LevelData {
        uint32_t level_width;
        uint32_t level_length;
        std::vector< Magpie::PixelData > pixel_data;

        // Loads level information from a file
        LevelData(const std::string &filename);
    };

    struct LevelLoader {

        /**
         * Given amap of all the transforms in the game and a vector of transforms,
         * returns a fector of Transform pointers to transforms with the given names
         */
        static std::vector< Scene::Transform* > get_animation_transforms( std::unordered_map< std::string, Scene::Transform * >& name_to_transform, std::vector< std::string > names);
  
        // Places an animated door into the scene
        static Door& create_animated_door(Door& door, Scene& scene, uint8_t customization_id, glm::vec3 position);

        //Loads an animated model identically to how we load scene data.
        static Scene::Transform* load_animated_model(Scene &scene, AnimatedModel& model, const ModelData* model_data,
            std::string model_name, std::string vao_key, Scene::Object::ProgramInfo program_info, const MeshBuffer* mesh_buffer);

        // Loads level blob files and builds the scene
        static Magpie::MagpieLevel* load(const Magpie::LevelData* level_data, Scene& scene, const MeshBuffer* mesh_buffer, 
            std::function< Scene::Object*(Scene &, Scene::Transform *, std::string const &) > const &on_object);
    
    };
}
