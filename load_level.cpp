#include "load_level.hpp"
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

std::map<uint8_t, std::string> Magpie::LevelLoader::purple_meshes = {
    // Non Collidable
    {3, "floor_purple_MSH"},
    {4, "door_purple_MSH"},
    {5, "painting_yellow_MSH"},
    {6, "gem_red_MSH"},
    // Collidable
    {16, "wall_purple_MSH"},
    {17, "4-corner_purple_MSH"},
    {18, "3-corner_purple_MSH"},
    {19, "2-corner_purple_MSH"},
    {20, "pedestal_basic_MSH"}
};

Magpie::LevelLoader::LevelLoader() {
    mesh_names.insert({0, purple_meshes});
};

Magpie::MagpieLevel* Magpie::LevelLoader::load(std::string const &filename, Scene *scene,const MeshBuffer* mesh_buffer, 
            std::function< Scene::Object*(Scene &, Scene::Transform *, std::string const &) > const &on_object) {

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
    pixel_data.resize(level_length * level_width);
    if (!file.read(reinterpret_cast< char * >(&pixel_data[0]), pixel_data.size() * sizeof(Magpie::PixelData))) {
		throw std::runtime_error("Failed to read pixel data.");
	}

    if (file.peek() != EOF) {
		std::cerr << "WARNING: trailing data in level file '" << filename << "'" << std::endl;
	}

    ///////////////////////////////////////////////////////
    //             PLACE OBJECTS IN SCENE                //
    ///////////////////////////////////////////////////////

    Magpie::MagpieLevel* level = new MagpieLevel(width, length);

    std::vector <Scene::Transform *> potential_wall_locations;
    std::vector <Scene::Transform *> potential_pedestal_locations;
    
    // Temporary pointers for tracking objects
    Scene::Transform *temp_transform;
    Scene::Object *temp_object;

    // Iterate along x-axis
    for (uint32_t y = 0; y < level_length; y++) {
        // Iterate along y-axis
        for (uint32_t x = 0; x < level_width; x++) {
            // Index of the tile in the vector
            uint32_t i = (y * level_width) + x;
            // Get the current pixel
            PixelData current_pixel = pixel_data[i];
            uint32_t room_number = current_pixel.get_room_number();
            uint32_t guard_number = current_pixel.guard_path_number();
            // Check the mesh value
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


                auto custom_mesh_grp = mesh_names.find(customization_id);
                if (custom_mesh_grp != mesh_names.end()) {
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

                // Get the meshes that surround this wall
                if (x == 0) {
                    temp_transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else if (x == level_width - 1) {
                    temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else if (y == 0 || y == level_length - 1) {
                    temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else {
                    uint8_t mesh_below = pixel_data[((y - 1) * level_width) + x].get_mesh_id();
                    uint8_t mesh_above = pixel_data[((y + 1) * level_width) + x].get_mesh_id();
                    uint8_t mesh_to_left = pixel_data[(y * level_width) + (x - 1)].get_mesh_id();
                    uint8_t mesh_to_right = pixel_data[(y * level_width) + (x + 1)].get_mesh_id();
                    
                    if ((mesh_to_left == 4 || (mesh_to_left >= 16 && mesh_to_left <= 19))
                        || (mesh_to_right == 4 || (mesh_to_right >= 16 && mesh_to_right <= 19))) {
                        //temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else if ((mesh_above == 4 || (mesh_above >= 16 && mesh_above <= 19))
                        || (mesh_below == 4 || (mesh_below >= 16 && mesh_below <= 19))) {
                        temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                }                
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
                    uint8_t mesh_below = pixel_data[((y - 1) * level_width) + x].get_mesh_id();
                    uint8_t mesh_above = pixel_data[((y + 1) * level_width) + x].get_mesh_id();
                    uint8_t mesh_to_left = pixel_data[(y * level_width) + (x - 1)].get_mesh_id();
                    uint8_t mesh_to_right = pixel_data[(y * level_width) + (x + 1)].get_mesh_id();
                    
                    if ((mesh_to_left == 4 || (mesh_to_left >= 16 && mesh_to_left <= 19))
                        || (mesh_to_right == 4 || (mesh_to_right >= 16 && mesh_to_right <= 19))) {
                        //temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else if ((mesh_above == 4 || (mesh_above >= 16 && mesh_above <= 19))
                        || (mesh_below == 4 || (mesh_below >= 16 && mesh_below <= 19))) {
                        temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                }
                
            }

            // 2-Corner
            if (mesh_id == 19) {
                std::string name = "2-corner_" + std::to_string(i);
                temp_transform->name = std::string(name);
                temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                
                
                // rotate corners
                if (x == 0) {
                    if (y == 0) {
                        // Do nothing, this is proper orientation
                    }
                    else if (y == level_length - 1) {
                        temp_transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else {
//                        uint8_t mesh_above = pixel_data[((y + 1) * level_width) + x].get_mesh_id();
                        uint8_t mesh_below = pixel_data[((y - 1) * level_width) + x].get_mesh_id();
                        if (mesh_below == 4 || (mesh_below >= 16 && mesh_below <= 19)) {
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
//                        uint8_t mesh_above = pixel_data[((y + 1) * level_width) + x].get_mesh_id();
                        uint8_t mesh_below = pixel_data[((y - 1) * level_width) + x].get_mesh_id();
                        if (mesh_below == 4 || (mesh_below >= 16 && mesh_below <= 19)) {
                            temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                        else {
                            temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                    }
                }
                else {
                    if (y == 0) {
                        uint8_t mesh_to_left = pixel_data[(y * level_width) + (x - 1)].get_mesh_id();
                        if (mesh_to_left == 4 || (mesh_to_left >= 16 && mesh_to_left<= 19)) {
                            temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                        }     
                    }
                    else if (y == level_length - 1) {
                        uint8_t mesh_to_left = pixel_data[(y * level_width) + (x - 1)].get_mesh_id();
                        if (mesh_to_left == 4 || (mesh_to_left >= 16 && mesh_to_left<= 19)) {
                            temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                        } else {
                            temp_transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                    }
                    else {
                        uint8_t mesh_to_left = pixel_data[(y * level_width) + (x - 1)].get_mesh_id();
                        uint8_t mesh_to_right = pixel_data[(y * level_width) + (x + 1)].get_mesh_id();
                        uint8_t mesh_to_top = pixel_data[((y + 1)* level_width) + x].get_mesh_id();
                        uint8_t mesh_to_bottom = pixel_data[((y - 1) * level_width) + x].get_mesh_id();

                        if ((mesh_to_left == 4 || (mesh_to_left >= 16 && mesh_to_left <= 19))) {
                            // Check to the top and bottom
                            if ((mesh_to_top == 4 || (mesh_to_top >= 16 && mesh_to_top <= 19))) {
                               temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                            } else {
                              temp_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                            }
                        }
                        else if ((mesh_to_right == 4 || (mesh_to_right >= 16 && mesh_to_right <= 19))) {
                            if ((mesh_to_bottom == 4 || (mesh_to_bottom >= 16 && mesh_to_bottom <= 19))) {
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
                        uint8_t mesh_to_bottom = pixel_data[((y - 1) * level_width) + x].get_mesh_id();
                        uint8_t mesh_to_left = pixel_data[(y * level_width) + (x - 1)].get_mesh_id();
                        uint8_t mesh_to_right = pixel_data[(y * level_width) + (x + 1)].get_mesh_id();

                        if ((mesh_to_left == 4 || (mesh_to_left >= 16 && mesh_to_left <= 19))
                            && (mesh_to_right == 4 || (mesh_to_right >= 16 && mesh_to_right <= 19))) {
                            if ((mesh_to_bottom == 4 || (mesh_to_bottom >= 16 && mesh_to_bottom <= 19))) {
                                 temp_transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                            }
                            else {
                                temp_transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                            }
                        }
                        else if ((mesh_to_left == 4 || (mesh_to_left >= 16 && mesh_to_left <= 19))) {
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

        auto custom_mesh_grp = mesh_names.find(0);
        if (custom_mesh_grp != mesh_names.end()) {
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
        
        auto custom_mesh_grp = mesh_names.find(0);
        if (custom_mesh_grp != mesh_names.end()) {
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