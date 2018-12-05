#include "load_level.hpp"
#include "Load.hpp"
#include "MagpieLevel.hpp"
#include "AssetLoader.hpp"
#include "AnimatedModel.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>



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

bool Magpie::PixelData::is_corner_3() {
    return get_mesh_id() == 18;
};

bool Magpie::PixelData::walls_to_left_and_right(std::vector< Magpie::PixelData > level_pixels, uint32_t level_width, uint32_t x, uint32_t y) {
    PixelData pixel_to_left = level_pixels[(y * level_width) + (x - 1)];
    PixelData pixel_to_right = level_pixels[(y * level_width) + (x + 1)];
    return pixel_to_left.is_wall_corner_door() && pixel_to_right.is_wall_corner_door();
};

bool Magpie::PixelData::walls_to_top_and_bottom(std::vector< Magpie::PixelData > level_pixels, uint32_t level_width, uint32_t x, uint32_t y) {
    PixelData pixel_below = level_pixels[((y - 1) * level_width) + x];
    PixelData pixel_above = level_pixels[((y + 1) * level_width) + x];
    return pixel_above.is_wall_corner_door() && pixel_below.is_wall_corner_door();
};

bool Magpie::PixelData::is_player_start_position() {
        uint8_t mesh_id = get_mesh_id();
        return mesh_id == 1;
};

bool Magpie::PixelData::is_guard_start_position() {
    uint8_t mesh_id = get_mesh_id();
    return mesh_id == 2;
};

Load< std::map < uint8_t, std::map< uint8_t, std::string > > > mesh_names(LoadTagDefault, [](){
    std::map< uint8_t, std::map<uint8_t, std::string > > *ret = new std::map< uint8_t, std::map<uint8_t, std::string > >();
    
    std::map<uint8_t, std::string > floors = {
        { 0, "floor_purple_MSH" },
        { 1, "floor_red_MSH" }
    };

    std::map<uint8_t, std::string > doors = {
        { 0, "door_purple_MSH" },
        { 1, "door_keycardPink_MSH" },
        { 2, "door_keycardGreen_MSH" },
        { 3, "door_backExit_MSH" },
        { 4, "door_frontExit1_MSH" },
        { 5, "door_frontExit2_MSH" },
        { 6, "door_frontExit3_MSH" }
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
        { 1, "wall_skeleton_MSH" },
        { 2, "wall_clock_MSH" },
        { 3, "wall_bulletin1_MSH" },
        { 4, "wall_bulletin2_MSH" }
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
        {0, "decoration_plant_MSH"},
        {1, "decoration_coatRack_MSH"},
        {2, "decoration_redRope1_MSH"},
        {3, "decoration_redRope2_MSH"},
        {4, "decoration_redRope3_MSH"}
    };

    std::map<uint8_t, std::string > security_rooms = {
        {0, "security_table_MSH"},
        {1, "security_computer_MSH"},
        {2, "security_monitor1_MSH"},
        {3, "security_monitor2_MSH"},
        {4, "security_locker_MSH"},
        {5, "security_table2_MSH"}
    };

    std::map<uint8_t, std::string > offices = {
        {0, "office_desk1_MSH"},
        {1, "office_desk2_MSH"},
        {2, "office_bookshelf_MSH"}
    };


    std::map<uint8_t, std::string > storage = {
        {0, "storage_crate_MSH"},
        {1, "storage_boxes1_MSH"},
        {2, "storage_boxes2_MSH"},
        {3, "storage_boxes3_MSH"}
    };

    std::map<uint8_t, std::string > chairs = {
        {0, "chair_plastic_MSH"},
        {1, "chair_plasticRotated_MSH"},
        {2, "chair_office_MSH"},
    };

    std::map<uint8_t, std::string > geodes = {
        {0, "geode_normal_MSH"},
        {1, "geode_bougie_MSH"}
    };

    std::map<uint8_t, std::string > keys = {
        {0, "keycard_pink_MSH"},
        {1, "keycard_green_MSH"},
        {2, "masterKey_pickup_MSH"}
    };

    std::map<uint8_t, std::string > treats = {
        {0, "dogTreat_MSH"},
        {1, "dogTreat_pickup_MSH"}
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
    ret->insert({26, chairs});
    ret->insert({42, keys});
    ret->insert({57, treats});
    ret->insert({69, geodes});
    

    return ret;
});

Magpie::LevelData::LevelData(const std::string &filename) {
    ///////////////////////////////////////////////////////
    //            READING FROM LEVEL FILE                //
    ///////////////////////////////////////////////////////
    std::ifstream file(filename, std::ios::binary);

    std::string magic = "levl";

    char level_file_header[5] = {'\0', '\0', '\0', '\0', '\0'};

    if(!file.read(reinterpret_cast< char* >(&level_file_header), sizeof(char) * 4)) {
        std::cout << "ERROR::load_level:: Can't read level file header." << std::endl;
    }

    int width;
    int length;

    if(file.read(reinterpret_cast< char* >(&length), sizeof(int))) {
        this->level_length = length;
    }

    if(file.read(reinterpret_cast< char* >(&width), sizeof(int))) {
        this->level_width = width;
    }

    std::vector< Magpie::PixelData > imported_pixel_data;
    imported_pixel_data.resize(length * width);
    if (!file.read(reinterpret_cast< char * >(&imported_pixel_data[0]), imported_pixel_data.size() * sizeof(Magpie::PixelData))) {
		throw std::runtime_error("Failed to read pixel data.");
	}
    this->pixel_data = imported_pixel_data;

    if (file.peek() != EOF) {
		std::cerr << "WARNING: trailing data in level file '" << filename << "'" << std::endl;
	}
};


Scene::Transform* Magpie::LevelLoader::load_animated_model(Scene& scene, AnimatedModel& model, const ModelData* model_data, std::string model_name, std::string vao_key,
        Scene::Object::ProgramInfo program_info, const MeshBuffer* mesh_buffer) {

    Scene::Transform* model_group_transform = nullptr;

    model_group_transform = model.load_model(scene, model_data, model_name, [=](Scene &s, Scene::Transform *t, std::string const &m){
        Scene::Object *obj = s.new_object(t);
        Scene::Object::ProgramInfo default_program_info;
        default_program_info = program_info;
        default_program_info.vao = vertex_color_vaos->find(vao_key)->second;
        obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
        MeshBuffer::Mesh const &mesh = mesh_buffer->lookup(m);
        obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
        obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        return obj;
    });

    assert(model_group_transform != nullptr);

    return model_group_transform;
};


Magpie::Door& Magpie::LevelLoader::create_front_door(Magpie::Door& door, Scene& scene, uint8_t customization_id, glm::vec3 position) {

    Scene::Transform* model_group_transform = nullptr;

    std::string model_name = "";
    switch(customization_id) {
        case 4:
            model_name = "frontExit1";
            break;
        case 5:
            model_name = "frontExit2";
            break;
        case 6:
            model_name = "frontExit3";
            break;
        default:
            std::cout << "ERROR::create_front_door:: Front door customization id not found" << std::endl;
            break;
    }

    model_group_transform = door.load_model(scene, front_door_model.value, model_name, [&](Scene &s, Scene::Transform *t, std::string const &m){
        switch(customization_id) {
            case 4:
                if (m.find("Exit1") != std::string::npos) {
                    Scene::Object *obj = s.new_object(t);
                    Scene::Object::ProgramInfo default_program_info;

                    if (m.find("glass") != std::string::npos) {
                        // Use transparent program for glass
                        default_program_info = *transparent_program_info.value;
                        default_program_info.vao = *transparent_building_meshes_vao;
                    }
                    else {
                        // Use vertex color program for frame
                        default_program_info = *vertex_color_program_info.value;
                        default_program_info.vao = vertex_color_vaos->find("buildingTiles")->second;
                    }

                    obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
                    MeshBuffer::Mesh const &mesh = building_meshes->lookup(m);
                    obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
                    obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
            
                    return obj;
                }
                break;
            case 5:
                if (m.find("Exit2") != std::string::npos) {
                    Scene::Object *obj = s.new_object(t);
                    Scene::Object::ProgramInfo default_program_info;

                    if (m.find("glass") != std::string::npos) {
                        // Use transparent program for glass
                        default_program_info = *transparent_program_info.value;
                        default_program_info.vao = *transparent_building_meshes_vao;
                    }
                    else {
                        // Use vertex color program for frame
                        default_program_info = *vertex_color_program_info.value;
                        default_program_info.vao = vertex_color_vaos->find("buildingTiles")->second;
                    }

                    obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
                    MeshBuffer::Mesh const &mesh = building_meshes->lookup(m);
                    obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
                    obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
                    return obj;
                }
                break;
            case 6:
                if (m.find("Exit3") != std::string::npos) {
                    Scene::Object *obj = s.new_object(t);
                    Scene::Object::ProgramInfo default_program_info;

                    if (m.find("glass") != std::string::npos) {
                        // Use transparent program for glass
                        default_program_info = *transparent_program_info.value;
                        default_program_info.vao = *transparent_building_meshes_vao;
                    }
                    else {
                        // Use vertex color program for frame
                        default_program_info = *vertex_color_program_info.value;
                        default_program_info.vao = vertex_color_vaos->find("buildingTiles")->second;
                    }

                    obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
                    MeshBuffer::Mesh const &mesh = building_meshes->lookup(m);
                    obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
                    obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
                    return obj;
                }
                break;
            default:
                std::cout << "ERROR::create_front_door:: Front door customization id not found" << std::endl;
                break;
        }
        // Delete the transform if it doesnt belong to any of the doors
        scene.delete_transform(t);
        return (Scene::Object *)nullptr;
    });
    

    assert(model_group_transform != nullptr);
    door.scene_object = scene.new_object(model_group_transform);
    door.set_transform(&door.scene_object->transform);
    if (model_group_transform->name.find("frontExit1") != std::string::npos) {
        door.set_position(glm::vec3(position.x + 1, position.y, position.z));
    }
    else if (model_group_transform->name.find("frontExit2") != std::string::npos) {
        door.set_position(position);
    }
    else if (model_group_transform->name.find("frontExit3") != std::string::npos) {
        door.set_position(glm::vec3(position.x - 1, position.y, position.z));
    }
    

    return door;
};


Magpie::Door& Magpie::LevelLoader::create_animated_door(Magpie::Door& door, Scene& scene, uint8_t customization_id, glm::vec3 position) {    

    // Load the model data for the door
    Scene::Transform *door_trans = nullptr;
    std::string model_name;
    switch(customization_id) {
        case 0:
            model_name = "PurpleDoor";
            door_trans = load_animated_model(scene, door, door_model.value, model_name, "door", *vertex_color_program_info.value, door_mesh.value);
            break;
        case 1:
            model_name = "PinkDoor";
            door_trans = load_animated_model(scene, door, door_pink_model.value, model_name, "door_pink", *vertex_color_program_info.value, door_pink_mesh.value);
            break;
        case 2:
            model_name = "GreenDoor";
            door_trans = load_animated_model(scene, door, door_green_model.value, model_name, "door_green", *vertex_color_program_info.value, door_green_mesh.value);
            break;
    }
     

    //look up various transforms for animations
    std::unordered_map< std::string, Scene::Transform * > name_to_transform;
    for (Scene::Transform *t = scene.first_transform; t != nullptr; t = t->alloc_next) {
        if (t->name.find(model_name) != std::string::npos) {
            auto ret = name_to_transform.insert(std::make_pair(t->name, t));
            if (!ret.second) {
                std::cerr << "WARNING: multiple transforms with the name '" << t->name << "' in scene." << std::endl;
            }
        }
    }

    std::vector< Scene::Transform* > door_transforms;
    switch(customization_id) {
        case 0:
            door_transforms = get_animation_transforms(name_to_transform, door.convert_animation_names(door_tanim.value, model_name));
            break;
        case 1:
            door_transforms = get_animation_transforms(name_to_transform, door.convert_animation_names(door_pink_tanim.value, model_name));
            break;
        case 2:
            door_transforms = get_animation_transforms(name_to_transform, door.convert_animation_names(door_green_tanim.value, model_name));
            break;
    }


    TransformAnimationPlayer* door_animation = nullptr;
    switch(customization_id) {
        case 0:
            door_animation = new TransformAnimationPlayer(*door_tanim, door_transforms, 1.0f, false);
            break;
        case 1:
            door_animation = new TransformAnimationPlayer(*door_pink_tanim, door_transforms, 1.0f, false);
            break;
        case 2:
            door_animation = new TransformAnimationPlayer(*door_green_tanim, door_transforms, 1.0f, false);
            break;
    }

    assert(door_trans != nullptr);
    assert(door_animation != nullptr);
    door.get_animation_manager()->add_state(new AnimationState(door_trans, door_animation));

    // Finally, set the transform for this guard
    door.set_transform(door.get_animation_manager()->init(position, 0));
    if (door.get_transform() == nullptr) {
        std::cerr << "ERROR:: Door Transform not found" << std::endl;
    }

    // Set the guard at the proper place
    door.set_position(position);
    
    return door;
};


std::vector< Scene::Transform* > Magpie::LevelLoader::get_animation_transforms( std::unordered_map< std::string, Scene::Transform * >& name_to_transform, std::vector< std::string > names) {

    std::vector< Scene::Transform* > animation_transforms;

    for (auto const &name : names) {
        auto f = name_to_transform.find(name);
        if (f == name_to_transform.end()) {
            std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
            animation_transforms.emplace_back(nullptr);
        } else {
            animation_transforms.emplace_back(f->second);
        }
    }

    return animation_transforms;
};


Magpie::MagpieLevel* Magpie::LevelLoader::load(const Magpie::LevelData* level_data, Scene &scene, const MeshBuffer* mesh_buffer, 
            std::function< Scene::Object*(Scene &, Scene::Transform *, std::string const &) > const &on_object) {

    ///////////////////////////////////////////////////////
    //             PLACE OBJECTS IN SCENE                //
    ///////////////////////////////////////////////////////

    Magpie::MagpieLevel* level = new MagpieLevel(level_data->level_width, level_data->level_length);

    // Lambda Function for Loading static geometry
    auto get_mesh = [&scene, &on_object](uint32_t x, uint32_t y, uint8_t mesh_id, uint8_t customization_id) {
        Scene::Transform *temp_transform = scene.new_transform();
        temp_transform->position.x = (float)x;
        temp_transform->position.y = (float)y;

        auto custom_mesh_grp = mesh_names->find(mesh_id);
        if (custom_mesh_grp != mesh_names->end()) {
            auto custom_mesh_name = custom_mesh_grp->second.find(customization_id);
            if (custom_mesh_name != custom_mesh_grp->second.end()) {
                return on_object(scene, temp_transform, custom_mesh_name->second);
            }
        }

        std::cout << "ERROR::GetMeshLambda:: Mesh ID " << mesh_id << "customization ID " << customization_id << "not found." << std::endl;
        return (Scene::Object *)nullptr;
    };

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
            bool potential_item_location = current_pixel.is_item_location();

            PixelData pixel_above;
            if (y < level_data->level_length - 1)
                pixel_above = level_data->pixel_data[((y + 1) * level_data->level_width) + x];
            
            PixelData pixel_below;
            if (y > 0)
                pixel_below = level_data->pixel_data[((y - 1) * level_data->level_width) + x];
            
            PixelData pixel_to_left;
            if (x > 0)
                pixel_to_left = level_data->pixel_data[(y * level_data->level_width) + (x - 1)];
            
            PixelData pixel_to_right;
            if (x < level_data->level_width - 1)
                pixel_to_right = level_data->pixel_data[(y * level_data->level_width) + (x + 1)];

            // Check if this is a start position for the player
            if (current_pixel.is_player_start_position()) {
                level->set_player_start_position(glm::vec3((float)x, (float)y, 0.0f));
            }

            // Check if this is a start position for a guard
            if (current_pixel.is_guard_start_position()) {
                GameAgent::DIRECTION dir = GameAgent::DIRECTION::RIGHT;
                switch (customization_id) {
                    case 1:
                        dir = GameAgent::DIRECTION::UP;
                        break;
                    case 2:
                        dir = GameAgent::DIRECTION::LEFT;
                        break;
                    case 3:
                        dir = GameAgent::DIRECTION::DOWN;
                        break;
                    default:
                        break;
                }

                level->add_guard_start_position(room_number, guard_number, glm::vec3((float)x, (float)y, 0.0f), dir);
                level->set_movement_matrix_position(x, y, true);

                Scene::Object* obj = get_mesh(x, y, 3, 0);
                obj->transform->name = "floor_" + std::to_string(i);
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                level->set_movement_matrix_position(x, y, true);
                FloorTile*** floor = level->get_floor_matrix();
                floor[x][y] = new FloorTile(obj, room_number);
            }

            // Check if it is part of a guards path
            if (guard_number != 0) {
                level->add_guard_path_position(room_number, guard_number, x, y);
                level->set_movement_matrix_position(x, y, true);

//                Scene::Object* obj = get_mesh(x, y, 3, 0);
//                obj->transform->name = "floor_" + std::to_string(i);
//                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
//                level->set_movement_matrix_position(x, y, true);
//                FloorTile*** floor = level->get_floor_matrix();
//                floor[x][y] = new FloorTile(obj, room_number);
            }
            
            // Floor Tile                           
            if (current_pixel.is_player_start_position() || mesh_id == 3) {
                Scene::Object* obj = get_mesh(x, y, 3, customization_id);
                obj->transform->name = "floor_" + std::to_string(i);
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                level->set_movement_matrix_position(x, y, true);
                FloorTile*** floor = level->get_floor_matrix();
                floor[x][y] = new FloorTile(obj, room_number);

                // Adds this transform to the vector
                // of locations where things can be placed
                // on the floor
                if (potential_item_location && level->cardboard_box == nullptr) {
                    Scene::Object* cardboardbox_obj = get_mesh(x, y, 25, 1);
                    cardboardbox_obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                    level->cardboard_box = new CardboardBox(cardboardbox_obj);
                }

            }

            // Doors
            else if (mesh_id == 4) {
                std::string name = "Door_" + std::to_string(i);
                
                Door* door = new Door();
                bool animated_door = false;

                if (customization_id >= 0 && customization_id <= 2) {
                    animated_door = true;
                    // Spawn an animated door model
                    create_animated_door(*door, scene, customization_id, glm::vec3((float)x, (float)y, 0.0f));
                    // Add door the the levels vector of doors
                    level->get_doors()->push_back(door); 
                } else if (customization_id == 3) {
                    // Spawn a static door
                    Scene::Object* obj = get_mesh(x, y, mesh_id, customization_id);
                    door->scene_object = obj;
                    door->set_transform(&obj->transform);
                    (*door->get_transform())->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                }
                else{
                    create_front_door(*door, scene, customization_id, glm::vec3((float)x, (float)y, 0.0f));
                    assert(*door->get_transform() != nullptr);
                }
                

                 

                // Sets the access level for the door based on the customization ID
                door->access_level = (Door::ACCESS_LEVEL)customization_id;
                
                // Get the meshes that surround this wall and rotate the wall
                if (x == 0) {
                    (*door->get_transform())->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    door->room_a = glm::ivec2(int(x) + 1, (int)y);
                    door->room_b = glm::ivec2(int(x) - 1, (int)y);
                }
                else if (x == level_data->level_width - 1) {
                    (*door->get_transform())->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                    door->room_a = glm::ivec2(int(x) - 1, (int)y);
                    door->room_b = glm::ivec2(int(x) + 1, (int)y);
                }
                else if (y == 0 || y == level_data->level_length - 1) {
                    (*door->get_transform())->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                    door->room_a = glm::ivec2(int(x), (int)y - 1);
                    door->room_b = glm::ivec2(int(x), (int)y + 1);
                }
                else {
                    if (PixelData::walls_to_left_and_right(level_data->pixel_data, level_data->level_width, x, y)) {
                        //(*door->get_transform())->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                        door->room_a = glm::ivec2(int(x), (int)y - 1);
                        door->room_b = glm::ivec2(int(x), (int)y + 1);
                    }
                    else if (PixelData::walls_to_top_and_bottom(level_data->pixel_data, level_data->level_width, x, y)) {
                        (*door->get_transform())->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                        door->room_a = glm::ivec2(int(x) - 1, (int)y);
                        door->room_b = glm::ivec2(int(x) + 1, (int)y);
                    }
                    else {
                        door->room_a = glm::ivec2(int(x), (int)y - 1);
                        door->room_b = glm::ivec2(int(x), (int)y + 1);
                    }
                }

                if (animated_door) {
                    PixelData pa = level_data->pixel_data[door->room_a.y * level_data->level_width + door->room_a.x];
                    PixelData pb = level_data->pixel_data[door->room_b.y * level_data->level_width + door->room_b.x];
                    door->rooms.insert({pa.get_room_number(), door->room_b});
                    door->rooms.insert({pb.get_room_number(), door->room_a});
                }                   
            }

             // Walls
            else if (mesh_id == 16) {
                Scene::Object* obj = get_mesh(x, y, mesh_id, customization_id);
                obj->transform->name = "wall_" + std::to_string(i);
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

                level->set_wall(new Wall(obj, room_number), x, y);
                
                if (current_pixel.is_item_location()) {
                    level->add_potential_location(level->get_potential_wall_locations(), room_number, obj->transform);
                }


                if (x == 0) {
                    obj->transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else if (x == level_data->level_width - 1) {
                    obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else if (y == 0) {
                    // Do Nothing
                }
                else if (y == level_data->level_length - 1) {
                    //obj->transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else {
                    // Get the meshes that surround this wall
                    if(PixelData::walls_to_left_and_right(level_data->pixel_data, level_data->level_width, x, y)) {
                        //obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else if (PixelData::walls_to_top_and_bottom(level_data->pixel_data, level_data->level_width, x, y)) {
                        obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                }
                
            }

            // 2-Corner
            else if (mesh_id == 19) {
                Scene::Object* obj = get_mesh(x, y, mesh_id, customization_id);
                std::string name = "2-corner_" + std::to_string(i);
                obj->transform->name = std::string(name);
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                level->set_wall(new Wall(obj, room_number), x, y);        

                // rotate corners
                if (x == 0) {
                    if (y == 0) {
                        // Do nothing, this is proper orientation (L)
                    }
                    else if (y == level_data->level_length - 1) {
                        obj->transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else {
                        if (pixel_below.is_wall_corner_door()) {
                            obj->transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                    }
                }
                else if (x == level_data->level_width - 1) {
                    if (y == 0) {
                        obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else if (y == level_data->level_length - 1) {
                        obj->transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else {
                        if (pixel_below.is_wall_corner_door()) {
                            obj->transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                        else {
                            obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                    }
                }
                else {
                    if (y == 0) {          
                        if (pixel_to_left.is_wall_corner_door()) {
                            obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                        }     
                    }
                    else if (y == level_data->level_length - 1) {
                        if (pixel_to_left.is_wall_corner_door()) {
                            obj->transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                        } else {
                            obj->transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                    }
                    else {
                        if (pixel_to_left.is_wall_corner_door() && pixel_above.is_wall_corner_door()) {
                            obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                        else if (pixel_to_left.is_wall_corner_door() && pixel_below.is_wall_corner_door()) {
                            obj->transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                        else if (pixel_to_right.is_wall_corner_door() && pixel_below.is_wall_corner_door()) {
                            obj->transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                    }
                }
            }

            // 3-Corner
            else if (mesh_id == 18) {
                Scene::Object* obj = get_mesh(x, y, mesh_id, customization_id);
                std::string name = "3-corner_" + std::to_string(i);
                obj->transform->name = std::string(name);
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                level->set_wall(new Wall(obj, room_number), x, y);

                // rotate corners
                if (x == 0) {
                    // Do Nothing
                }
                else if (x == level_data->level_width - 1) {
                    obj->transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else {
                    if (y == 0) {
                        obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else if (y == level_data->level_length - 1) {
                        obj->transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                    }
                    else {
                        if (pixel_to_left.is_wall_corner_door() && pixel_to_right.is_wall_corner_door()) {
                            if (pixel_below.is_wall_corner_door()) {
                                 obj->transform->rotation *= glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
                            }
                            else {
                                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                            }
                        }
                        else if (pixel_to_left.is_wall_corner_door()) {
                            obj->transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                        else {
                            // Do nothing
                        }
                    }
                }

                if (current_pixel.is_item_location()) {
                    level->add_potential_location(level->get_potential_wall_locations(), room_number, obj->transform);
                }
            }

            // 4-Corner
            else if (mesh_id == 17) {
                Scene::Object* obj = get_mesh(x, y, mesh_id, customization_id);
                std::string name = "4-corner_" + std::to_string(i);
                obj->transform->name = std::string(name);
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                level->set_wall(new Wall(obj, room_number), x, y);
            }

            // Pedestal
            else if (mesh_id == 20) {
                Scene::Object* obj = get_mesh(x, y, mesh_id, customization_id);
                std::string name = "pedestal_" + std::to_string(i);
                obj->transform->name = name;
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

                // Adds this transform to the vector
                // of locations where things can be placed
                // on the floor
                if (potential_item_location) {
                    level->add_potential_location(level->get_potential_pedestal_locations(), room_number, obj->transform);
                }
            }
    
            else if (mesh_id == 21) {
                DisplayCase* displaycase = new DisplayCase();
                Scene::Transform* display_group = nullptr;
                switch(customization_id) {
                    case 0:
                        display_group = displaycase->load_model(scene, front_door_model.value, "displayCaseWhole", [&](Scene &s, Scene::Transform *t, std::string const &m){
                            
                            if( t->name.find("displayCaseWhole") != std::string::npos) {
                                Scene::Object *obj = s.new_object(t);

                                Scene::Object::ProgramInfo default_program_info;
                                default_program_info = *vertex_color_program_info.value;
                                default_program_info.vao = vertex_color_vaos->find("buildingTiles")->second;

                                obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
                                MeshBuffer::Mesh const &mesh = building_meshes->lookup(m);
                                obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
                                obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
                                return obj;
                            } else {
                                scene.delete_transform(t);
                            }
                            return (Scene::Object*)nullptr;
                        });
                        break;
                    case 1:
                        display_group = displaycase->load_model(scene, front_door_model.value, "displayCaseBroken", [&](Scene &s, Scene::Transform *t, std::string const &m){
                            if( t->name.find("displayCaseBroken") != std::string::npos) {
                                Scene::Object *obj = s.new_object(t);

                                Scene::Object::ProgramInfo default_program_info;
                                default_program_info = *vertex_color_program_info.value;
                                default_program_info.vao = vertex_color_vaos->find("buildingTiles")->second;

                                obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
                                MeshBuffer::Mesh const &mesh = building_meshes->lookup(m);
                                obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
                                obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
                                return obj;
                            } else {
                                scene.delete_transform(t);
                            }
                            return (Scene::Object*)nullptr;
                        });
                        break;
                    default:
                        std::cout << "ERROR::load_level::  Invalid DisplayCase customization ID" << std::endl;
                        break;
                }

                displaycase->scene_object = scene.new_object(display_group);
                displaycase->set_transform(&displaycase->scene_object->transform);
                //(*displaycase->get_transform())->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                displaycase->set_position(glm::vec3((float)x, (float)y, 0.0f));
                level->add_displaycase(displaycase);
                // Place geode
                Scene::Object* obj = get_mesh(x, y, 69, 0);
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                displaycase->geode = new Geode(obj);

                Scene::Object* floor_obj = get_mesh(x, y, 3, 1);
                floor_obj->transform->name = "floor_" + std::to_string(i);
                floor_obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

            }
            

            else if (mesh_id == 22) {
                Scene::Object* obj = get_mesh(x, y, mesh_id, customization_id);
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            }

            // SECURITY OFFICE
            else if (mesh_id == 23) {

                Scene::Object* obj = get_mesh(x, y, mesh_id, customization_id);
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

                if (customization_id == 0 && level->pink_card == nullptr && potential_item_location) {
                    Scene::Object* keycard_obj = get_mesh(x, y, 42, 0);
                    keycard_obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                    level->pink_card = new KeyCard(Door::ACCESS_LEVEL::PINK, keycard_obj);
                }

                if (customization_id == 5 && level->pink_card == nullptr && potential_item_location) {
                    Scene::Object* dogTreatPickup_obj = get_mesh(x, y, 57, 1);
                    dogTreatPickup_obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                    level->dogTreatPickUp = new DogTreat(dogTreatPickup_obj);
                }
            }

            // OFFICES
            else if (mesh_id == 24) {
                Scene::Object* obj = get_mesh(x, y, mesh_id, customization_id);
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

                if (customization_id == 0 && level->green_card == nullptr) {
                    Scene::Object* keycard_obj = get_mesh(x, y, 42, 1);
                    keycard_obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                    level->green_card = new KeyCard(Door::ACCESS_LEVEL::GREEN, keycard_obj);
                }
            }
            //STORAGE
            else if (mesh_id == 25) {
                Scene::Object* obj = get_mesh(x, y, mesh_id, customization_id);
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                if (customization_id == 0 && level->master_key == nullptr) {
                    Scene::Object* keycard_obj = get_mesh(x, y, 42, 2);
                    keycard_obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
                    level->master_key = new KeyCard(Door::ACCESS_LEVEL::MASTER, keycard_obj);
                }
            }

            else if (mesh_id == 26) {
                Scene::Object* obj = get_mesh(x, y, mesh_id, customization_id);
                obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            }

        }
    }

    std::mt19937 mt_rand ((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());

    for (auto const &room : *(level->get_potential_pedestal_locations())) {
        for (auto const &location: room.second) {
            uint32_t cust_id = mt_rand() % 3;
            Scene::Object* obj = get_mesh((uint32_t)location->position.x, (uint32_t)location->position.y, 6, cust_id);
            assert(obj != nullptr);
            obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            Gem* gem = new Gem(obj);
            obj->transform->name = "Gem" + std::to_string(gem->get_instance_id());
            level->add_gem(room.first, gem);    
        }
    };

    for (auto const &room : *(level->get_potential_wall_locations())) {
        for (auto const &location: room.second) {
            // random customization id;
            uint32_t cust_id = mt_rand() % 3;
            Scene::Object* obj = get_mesh((uint32_t)location->position.x, (uint32_t)location->position.y, 5, cust_id);
            assert(obj != nullptr);
            obj->transform->rotation = location->rotation;
            //obj->transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
            obj->transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
            //obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            PixelData pd = level_data->pixel_data[(uint32_t)location->position.y * level_data->level_width + (uint32_t)location->position.x];
            if(pd.is_corner_3()) {
                obj->transform->rotation *= glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
            }
            Painting* painting = new Painting(obj);
            obj->transform->name = "Painting" + std::to_string(painting->get_instance_id());
            level->add_painting(room.first, painting);    
        }
    };

    return level;
}