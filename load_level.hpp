// Loads level blob files and builds the scene

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

        // Helper methods for extracting data
        uint8_t get_mesh_id();
        uint8_t get_room_number();
        uint8_t is_guard_path();
        uint8_t get_object_id();
        uint8_t get_interaction_func_id();
        uint8_t get_interaction_flag();

        // MASK VALUES
        static uint8_t COLLIDABLE_MESH_MASK;
        static uint8_t NONCOLLIDABLE_MESH_MASK;
        static uint8_t ROOM_NUMBER_MASK;
        static uint8_t GUARD_PATH_MASK;
        static uint8_t OBJECT_ID_MASK;
        static uint8_t INTERACTION_FUNC_ID_MASK;
        static uint8_t INTERACTION_FLAG_MASK;

        // OFFSET VALUES
        static uint8_t COLLIDABLE_MESH_OFFSET;
        static uint8_t NONCOLLIDABLE_MESH_OFFSET;
        static uint8_t ROOM_NUMBER_OFFSET;
        static uint8_t GUARD_PATH_OFFSET;
        static uint8_t OBJECT_ID_OFFSET;
        static uint8_t INTERACTION_FUNC_ID_OFFSET;
        static uint8_t INTERACTION_FLAG_OFFSET;

    };

    class LevelLoader {
    public:
        static std::map<uint8_t, std::string> mesh_id;

        void load(std::string const &level_filename);
    };
}
