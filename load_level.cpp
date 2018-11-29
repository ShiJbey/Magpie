#include "load_level.hpp"
#include "load.hpp"
#include "MagpieLevel.hpp"

#include <iostream>
#include <fstream>
#include <vector>


uint8_t Magpie::PixelData::MESH_MASK = 0b11111000;
uint8_t Magpie::PixelData::MESH_CUSTOMIZATION_MASK = 0b00000111;
uint8_t Magpie::PixelData::ROOM_NUMBER_MASK = 0b11111000;
uint8_t Magpie::PixelData::GUARD_PATH_MASK = 0b00000110;
uint8_t Magpie::PixelData::ITEM_LOCATION_MASK = 0b00000001;
uint8_t Magpie::PixelData::OBJECT_ID_MASK = 0b00001111;
uint8_t Magpie::PixelData::GROUP_ID_MASK = 0b11110000;

uint8_t Magpie::PixelData::MESH_OFFSET = 3;
uint8_t Magpie::PixelData::MESH_CUSTOMIZATION_OFFSET = 0;
uint8_t Magpie::PixelData::ROOM_NUMBER_OFFSET = 3;
uint8_t Magpie::PixelData::GUARD_PATH_OFFSET = 1;
uint8_t Magpie::PixelData::ITEM_LOCATION_OFFSET = 0;
uint8_t Magpie::PixelData::OBJECT_ID_OFFSET = 0;
uint8_t Magpie::PixelData::GROUP_ID_OFFSET = 4;

Magpie::PixelData::PixelData() {
    red_channel_data = 0;
    green_channel_data = 0;
    blue_channel_data = 0;
}

Magpie::PixelData::PixelData(uint8_t red_channel_data_, uint8_t green_channel_data_, uint8_t blue_channel_data_) {
    red_channel_data = red_channel_data_;
    green_channel_data = green_channel_data_;
    blue_channel_data = blue_channel_data_;
}

std::string Magpie::PixelData::to_string() {
    return "[" + std::to_string(red_channel_data) + ", " + std::to_string(green_channel_data) + ", " + std::to_string(blue_channel_data) + "]";
}

uint8_t Magpie::PixelData::get_mesh_id() {
    return (red_channel_data & MESH_MASK) >> MESH_OFFSET;
}

uint8_t Magpie::PixelData::get_mesh_customization() {
    return (red_channel_data & MESH_CUSTOMIZATION_MASK) >> MESH_CUSTOMIZATION_OFFSET;
}

uint8_t Magpie::PixelData::get_room_number() {
    return (green_channel_data & ROOM_NUMBER_MASK) >> ROOM_NUMBER_OFFSET;
}

uint8_t Magpie::PixelData::guard_path_number() {
    return (green_channel_data & GUARD_PATH_MASK) >> GUARD_PATH_OFFSET;
}

bool Magpie::PixelData::is_item_location() {
    return (bool)((green_channel_data & ITEM_LOCATION_MASK) >> ITEM_LOCATION_OFFSET);
}

uint8_t Magpie::PixelData::get_object_id() {
    return (blue_channel_data & OBJECT_ID_MASK) >> OBJECT_ID_OFFSET;
}

uint8_t Magpie::PixelData::get_group_id() {
    return (blue_channel_data & GROUP_ID_MASK) >> GROUP_ID_OFFSET;
}

bool Magpie::PixelData::is_wall_corner_door() {
    uint8_t mesh_id = get_mesh_id();
    return mesh_id == 4 || (mesh_id >= 16 && mesh_id <= 19);
};

bool Magpie::PixelData::walls_to_left_and_right(std::vector< Magpie::PixelData >* level_pixels, uint32_t level_width, uint32_t x, uint32_t y) {
    PixelData pixel_to_left = (*level_pixels)[(y * level_width) + (x - 1)];
    PixelData pixel_to_right = (*level_pixels)[(y * level_width) + (x + 1)];
    return pixel_to_left.is_wall_corner_door() && pixel_to_right.is_wall_corner_door();
};

bool Magpie::PixelData::walls_to_top_and_bottom(std::vector< Magpie::PixelData >* level_pixels, uint32_t level_width, uint32_t x, uint32_t y) {
    PixelData pixel_below = (*level_pixels)[((y - 1) * level_width) + x];
    PixelData pixel_above = (*level_pixels)[((y + 1) * level_width) + x];
    return pixel_above.is_wall_corner_door() && pixel_below.is_wall_corner_door();
};

Load< std::map < uint8_t, std::map< uint8_t, std::string > > > mesh_names(LoadTagDefault, [](){
    std::map< uint8_t, std::map<uint8_t, std::string > > *ret = new std::map< uint8_t, std::map<uint8_t, std::string > >();
    
    std::map<uint8_t, std::string > floors = {
        { 0, "floor_purple_MSH" },
        { 1, "floor_red_MSH" }
    };

    std::map<uint8_t, std::string > doors = {
        { 0, "floor_purple_MSH" },
        { 1, "floor_keycardPink_MSH" },
        { 2, "floor_keycardGreen_MSH" },
        { 3, "floor_backExit_MSH" },
        { 4, "floor_frontExit1_MSH" },
        { 5, "floor_frontExit2_MSH" },
        { 6, "floor_frontExit3_MSH" }
    };

    std::map<uint8_t, std::string > paintings = {
        { 0, "painting_yellow_MSH" },
        { 1, "painting_scream_MSH" },
        { 2, "painting_rothko_MSH" }
    };

    std::map<uint8_t, std::string > gems = {
        { 0, "gem_red_MSH" },
        { 1, "gem_purple_MSH" },
        { 2, "gem_green_MSH" }
    };

    std::map<uint8_t, std::string > walls = {
        { 0, "wall_purple_MSH" },
        { 1, "wall_skeleton_MSH" }
    };

    std::map<uint8_t, std::string > four_sided_corners = {
        { 0, "4-corner_purple_MSH" },
        { 1, "4-corner_skeleton_MSH" }
    };

    std::map<uint8_t, std::string > three_sided_corners = {
        { 0, "3-corner_purple_MSH" },
        { 1, "3-corner_skeleton_MSH" }
    };

    std::map<uint8_t, std::string > two_sided_corners = {
        { 0, "2-corner_purple_MSH" },
        { 1, "2-corner_skeleton_MSH" }
    };

    std::map<uint8_t, std::string > pedestals = {
        {0, "pedestal_basic_MSH"}
    };

    std::map<uint8_t, std::string > display_cases = {
        {0, "displayCaseWhole_GRP"},
        {1, "displayCaseBroken_GRP"}
    };

    std::map<uint8_t, std::string > decorations = {
        {0, "decoration_plant_MSH"}
    };

    std::map<uint8_t, std::string > security_rooms = {
        {0, "security_table_MSH"},
        {1, "security_computer_MSH"},
        {2, "security_monitor1_MSH"},
        {3, "security_monitor2_MSH"}
    };

    std::map<uint8_t, std::string > offices = {
        {0, "office_desk1_MSH"},
        {1, "office_desk2_MSH"}
    };


    std::map<uint8_t, std::string > storage = {
        {0, "storage_crate_MSH"},
    };

    std::map<uint8_t, std::string > chairs = {
        {0, "chair_plastic_MSH"},
        {1, "chair_plasticRotated_MSH"},
        {2, "chair_office_MSH"},
    };

    ret->insert({3, floors});
    ret->insert({4, doors});
    ret->insert({5, paintings});
    ret->insert({6, gems});
    ret->insert({16, walls});
    ret->insert({17, four_sided_corners});
    ret->insert({18, three_sided_corners});
    ret->insert({19, two_sided_corners});
    ret->insert({20, pedestals});
    ret->insert({21, display_cases});
    ret->insert({22, decorations});
    ret->insert({23, security_rooms});
    ret->insert({24, offices});
    ret->insert({25, storage});
    ret->insert({22, chairs});
    

    return ret;
});

Magpie::LevelLoader::LevelLoader() {
    // Do Nothing
};

Magpie::LevelData::LevelData(const std::string &filename) {
    ///////////////////////////////////////////////////////
    //            READING FROM LEVEL FILE                //
    ///////////////////////////////////////////////////////
    std::ifstream file(filename, std::ios::binary);

    std::string magic = "levl";

    char level_file_header[5] = {'\0', '\0', '\0', '\0', '\0'};

    if(!file.read(reinterpret_cast< char* >(&level_file_header), sizeof(char) * 4)) {
        std::cout << "ERROR:: Can't read level file header" << std::endl;
    }

    int width;
    int length;

    if(file.read(reinterpret_cast< char* >(&length), sizeof(int))) {
        this->level_length = length;
    }

    if(file.read(reinterpret_cast< char* >(&width), sizeof(int))) {
        this->level_width = width;
    }

    std::vector< Magpie::PixelData > pixel_data;
    pixel_data.resize(length * width);
    if (!file.read(reinterpret_cast< char * >(&pixel_data[0]), pixel_data.size() * sizeof(Magpie::PixelData))) {
		throw std::runtime_error("Failed to read pixel data.");
	}
    this->pixel_data = pixel_data;

    if (file.peek() != EOF) {
		std::cerr << "WARNING: trailing data in level file '" << filename << "'" << std::endl;
	}
};


Magpie::MagpieLevel* Magpie::LevelLoader::load(const Magpie::LevelData* level_data, Scene *scene, const MeshBuffer* mesh_buffer, 
            std::function< Scene::Object*(Scene &, Scene::Transform *, std::string const &) > const &on_object) {

    ///////////////////////////////////////////////////////
    //             PLACE OBJECTS IN SCENE                //
    ///////////////////////////////////////////////////////

    Magpie::MagpieLevel* level = new MagpieLevel(level_data->level_width, level_data->level_length);

    std::vector <Scene::Transform *> potential_wall_locations;
    std::vector <Scene::Transform *> potential_pedestal_locations;
    
    // Temporary pointers for tracking objects
    Scene::Transform *temp_transform;
    Scene::Object *temp_object;

    // Iterate along x-axis
    for (uint32_t y = 0; y < level_data->level_length; y++) {
        // Iterate along y-axis
        for (uint32_t x = 0; x < level_data->level_width; x++) {

            // Index of the tile in the vector
            uint32_t i = (y * level_data->level_width) + x;
            
            // Get the current pixel and associated attributes
            PixelData current_pixel = level_data->pixel_data[i];
            
            uint32_t room_number = current_pixel.get_room_number();
            uint32_t guard_number = current_pixel.guard_path_number();
            uint8_t mesh_id = current_pixel.get_mesh_id();
            uint8_t customization_id = current_pixel.get_mesh_customization();

            // Check if it is part of a guards path
            if (guard_number != 0) {
                level->add_guard_path_position(room_number, guard_number, x, y);
            }

            // Create a new transform, give it a position, and attatch a mesh
            if (mesh_id != 0) {
                temp_transform = scene->new_transform();
                temp_transform->position.x = (float)x;
                temp_transform->position.y = (float)y;


                auto custom_mesh_grp = mesh_names->find(customization_id);
                if (custom_mesh_grp != mesh_names->end()) {
                    auto custom_mesh_name = custom_mesh_grp->second.find(mesh_id);
                    if (custom_mesh_name != custom_mesh_grp->second.end()) {
                        temp_object = on_object(*scene, temp_transform, custom_mesh_name->second);
                    }
                }
            }
            
            // Floor Tile                           
            if (mesh_id == 3) {
                temp_transform->name = "floor_" + std::to_string(i);
                temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                level->set_movement_matrix_position(x, y, true);
                FloorTile*** floor = level->get_floor_matrix();
                floor[x][y] = new FloorTile(temp_object, room_number);
                continue;
            }

            // Doors
            if (mesh_id == 4) {
                std::string name = "Door_" + std::to_string(i);
                temp_transform->name = name;
                temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                level->set_movement_matrix_position(x, y, true);
                Door* door;
                
                // Get the meshes that surround this wall
                if (x == 0) {
                    temp_transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                    door = new Door(glm::ivec2(int(x) + 1, (int)y), glm::ivec2(int(x) - 1, (int)y), temp_object);
                }
                else if (x == level_width - 1) {
                    temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    door = new Door(glm::ivec2(int(x) - 1, (int)y), glm::ivec2(int(x) + 1, (int)y), temp_object);
                }
                else if (y == 0 || y == level_length - 1) {
                    temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                    door = new Door(glm::ivec2(int(x), (int)y - 1), glm::ivec2(int(x), (int)y + 1), temp_object);
                }
                else {
                    if (PixelData::walls_to_left_and_right(&pixel_data, level_width, x, y)) {
                        temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                        door = new Door(glm::ivec2(int(x), (int)y - 1), glm::ivec2(int(x), (int)y + 1), temp_object);
                    }
                    else if (PixelData::walls_to_top_and_bottom(&pixel_data, level_width, x, y)) {
                        temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                        door = new Door(glm::ivec2(int(x) - 1, (int)y), glm::ivec2(int(x) + 1, (int)y), temp_object);
                    }
                    else {
                        door = new Door(glm::ivec2(int(x), (int)y - 1), glm::ivec2(int(x), (int)y + 1), temp_object);
                    }
                }


                level->get_doors()->push_back(door);                         
            }

             // Walls
            if (mesh_id == 16) {
                temp_transform->name = "wall_" + std::to_string(i);
                temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

                level->set_wall(new Wall(temp_object), x, y);
                
                if (current_pixel.is_item_location()) {
                    potential_wall_locations.push_back(temp_transform);
                }


                if (x == 0) {
                    temp_transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else if (x == level_width - 1) {
                    temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else if (y == 0) {
                    // Do Nothing
                }
                else if (y == level_length - 1) {
                    temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else {
                    // Get the meshes that surround this wall
                    if(PixelData::walls_to_left_and_right(&pixel_data, level_width, x, y)) {
                        //temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else if (PixelData::walls_to_top_and_bottom(&pixel_data, level_width, x, y)) {
                        temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                }
                
            }

            // 2-Corner
            if (mesh_id == 19) {
                std::string name = "2-corner_" + std::to_string(i);
                temp_transform->name = std::string(name);
                temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                
                
                PixelData* pixel_above = pixel_data[((y + 1) * level_width) + x];
                PixelData pixel_below = pixel_data[((y - 1) * level_width) + x];
                PixelData pixel_to_left = pixel_data[(y * level_width) + (x - 1)];
                PixelData pixel_to_right = pixel_data[(y * level_width) + (x + 1)];

                // rotate corners
                if (x == 0) {
                    if (y == 0) {
                        // Do nothing, this is proper orientation
                    }
                    else if (y == level_length - 1) {
                        temp_transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else {
                        if (pixel_below.is_wall_corner_door()) {
                            temp_transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                    }
                }
                else if (x == level_width - 1) {
                    if (y == 0) {
                        temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else if (y == level_length - 1) {
                        temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else {
                        if (pixel_below.is_wall_corner_door()) {
                            temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                        else {
                            temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                    }
                }
                else {
                    if (y == 0) {
                        
                        if (pixel_to_left.is_wall_corner_door()) {
                            temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                        }     
                    }
                    else if (y == level_length - 1) {
                        if (pixel_to_left.is_wall_corner_door()) {
                            temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                        } else {
                            temp_transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                    }
                    else {
                        if (PixelData::walls_to_left_and_right(&pixel_data, level_width, x, y)) {
                            // Check to the top and bottom
                            if (pixel_above.is_wall_corner_door()) {
                               temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                            } else {
                              temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                            }
                        }
                        else if (pixel_to_right.is_wall_corner_door()) {
                            if (pixel_below.is_wall_corner_door()) {
                               temp_transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                            }
                        }
                    }
                }
            }

            // 3-Corner
            if (mesh_id == 18) {
                std::string name = "3-corner_" + std::to_string(i);
                temp_transform->name = std::string(name);
                temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                
                PixelData pixel_above = pixel_data[((y + 1) * level_width) + x];
                PixelData pixel_below = pixel_data[((y - 1) * level_width) + x];
                PixelData pixel_to_left = pixel_data[(y * level_width) + (x - 1)];
                PixelData pixel_to_right = pixel_data[(y * level_width) + (x + 1)];

                // rotate corners
                if (x == 0) {
                    // Do Nothing
                }
                else if (x == level_width - 1) {
                    temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else {
                    if (y == 0) {
                        temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else if (y == level_length - 1) {
                        temp_transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else {
                        if (pixel_to_left.is_wall_corner_door() && pixel_to_right.is_wall_corner_door()) {
                            if (pixel_below.is_wall_corner_door()) {
                                 temp_transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                            }
                            else {
                                temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                            }
                        }
                        else if (pixel_to_left.is_wall_corner_door()) {
                            temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                        else {
                            // Do nothing
                        }
                    }
                }
            }

            // 4-Corner
            if (mesh_id == 17) {
                std::string name = "4-corner_" + std::to_string(i);
                temp_transform->name = std::string(name);
                temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            }

            // Pedestal
            if (mesh_id == 20) {
                std::string name = "pedestal_" + std::to_string(i);
                temp_transform->name = name;
                temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

                if (current_pixel.is_item_location()) {
                    potential_pedestal_locations.push_back(temp_transform);
                }
            }
        }
    }

    for (uint32_t i = 0; i < potential_pedestal_locations.size(); i++) {
        temp_transform = scene->new_transform();
        temp_transform->name = "gem_" + std::to_string(i);
        temp_transform->position.x = potential_pedestal_locations[i]->position.x;
        temp_transform->position.y = potential_pedestal_locations[i]->position.y;
        temp_transform->rotation = potential_pedestal_locations[i]->rotation;

        auto custom_mesh_grp = mesh_names->find(0);
        if (custom_mesh_grp != mesh_names->end()) {
            auto custom_mesh_name = custom_mesh_grp->second.find(6);
            if (custom_mesh_name != custom_mesh_grp->second.end()) {

                Scene::Object* obj = on_object(*scene, temp_transform, custom_mesh_name->second);
                // TODO:: Get the actual room
                level->add_gem(1, Gem(obj));
                assert(obj != nullptr);
            }
        }
    }

    for (uint32_t i = 0; i < potential_wall_locations.size(); i++) {
        temp_transform = scene->new_transform();
        temp_transform->name = "painting_" + std::to_string(i);
        temp_transform->position.x = potential_wall_locations[i]->position.x;
        temp_transform->position.y = potential_wall_locations[i]->position.y;
        temp_transform->rotation = potential_wall_locations[i]->rotation;
        
        auto custom_mesh_grp = mesh_names->find(0);
        if (custom_mesh_grp != mesh_names->end()) {
            auto custom_mesh_name = custom_mesh_grp->second.find(5);
            if (custom_mesh_name != custom_mesh_grp->second.end()) {
                
                Scene::Object* obj = on_object(*scene, temp_transform, custom_mesh_name->second);
                // TODO:: Get the actual room
                level->add_painting(1, Painting(obj));
            }
        }
    }

    return level;
}