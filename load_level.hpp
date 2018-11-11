#pragma once

#include "MagpieGame.hpp"
#include "Scene.hpp"
#include "MeshBuffer.hpp"

#include<map>
#include <string>


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
        // Will the guard use this postion to navigate
        uint8_t guard_path();

        // NOTE: The following are for implementing high level mechanics
        // Does the mesh at this position have a special identifier
        uint8_t get_object_id();
        uint8_t get_group_id();
        // What interaction lambda function is associated with this game object
        uint8_t get_interaction_func_id();
        // Does the interaction function need to set or check a global flag
        uint8_t get_interaction_flag();

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

    class LevelLoader {
    public:

        // just making each map static for now
        static std::map<uint8_t, std::string> purple_meshes;

        // maps customization numbers to maps of mesh IDs
        // mapped to the name of the mesh
        std::map <uint8_t, std::map<uint8_t, std::string>> mesh_names;

        // Dimensions of the level
        uint32_t level_width;
        uint32_t level_length;

        LevelLoader();

        // Loads level blob files and builds the scene
        void load(std::string const &level_filename, MagpieGame* game, Scene* scene, const MeshBuffer* mesh_buffer, 
            std::function< Scene::Object*(Scene &, Scene::Transform *, std::string const &) > const &on_object);
    };
}
