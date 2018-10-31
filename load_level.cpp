#include "load_level.hpp"
#include <iostream>
#include <fstream>
#include <vector>

uint8_t Magpie::PixelData::MESH_MASK = 0b11111000;
uint8_t Magpie::PixelData::MESH_CUSTOMIZATION_MASK = 0b00000111;
uint8_t Magpie::PixelData::ROOM_NUMBER_MASK = 0b11111000;
uint8_t Magpie::PixelData::GUARD_PATH_MASK = 0b00000110;
uint8_t Magpie::PixelData::ITEM_LOCATION_MASK = 0b00000001;
uint8_t Magpie::PixelData::OBJECT_ID_MASK = 0b01110000;
uint8_t Magpie::PixelData::INTERACTION_FUNC_ID_MASK = 0b00001100;
uint8_t Magpie::PixelData::INTERACTION_FLAG_MASK = 0b00000011;

uint8_t Magpie::PixelData::MESH_OFFSET = 3;
uint8_t Magpie::PixelData::MESH_CUSTOMIZATION_OFFSET = 0;
uint8_t Magpie::PixelData::ROOM_NUMBER_OFFSET = 3;
uint8_t Magpie::PixelData::GUARD_PATH_OFFSET = 1;
uint8_t Magpie::PixelData::ITEM_LOCATION_OFFSET = 0;
uint8_t Magpie::PixelData::OBJECT_ID_OFFSET = 4;
uint8_t Magpie::PixelData::INTERACTION_FUNC_ID_OFFSET = 2;
uint8_t Magpie::PixelData::INTERACTION_FLAG_OFFSET = 0;

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
    char buffer[50];
    sprintf(buffer, "[ %d, %d, %d ]", red_channel_data, green_channel_data, blue_channel_data);
    return std::string(buffer);
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

bool Magpie::PixelData::is_guard_path() {
    return (bool)((green_channel_data & GUARD_PATH_MASK) >> GUARD_PATH_OFFSET);
}

bool Magpie::PixelData::is_item_location() {
    return (bool)((green_channel_data & ITEM_LOCATION_MASK) >> ITEM_LOCATION_OFFSET);
}

uint8_t Magpie::PixelData::get_object_id() {
    return (blue_channel_data & OBJECT_ID_MASK) >> OBJECT_ID_OFFSET;
}

uint8_t Magpie::PixelData::get_interaction_func_id() {
    return (blue_channel_data & INTERACTION_FUNC_ID_MASK) >> INTERACTION_FUNC_ID_OFFSET;
}

uint8_t Magpie::PixelData::get_interaction_flag() {
    return (blue_channel_data & INTERACTION_FLAG_MASK) >> INTERACTION_FLAG_OFFSET;
}


static std::map<uint8_t, std::string> purple_meshes = {
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
}

void Magpie::LevelLoader::load(std::string const &filename) {
    std::ifstream file(filename, std::ios::binary);

    std::string magic = "levl";

    char level_file_header[5] = {'\0', '\0', '\0', '\0', '\0'};

    if(file.read(reinterpret_cast< char* >(&level_file_header), sizeof(char) * 4)) {
        std::cout << level_file_header << std::endl;
    }

    int level_width;
    int level_length;

    if(file.read(reinterpret_cast< char* >(&level_length), sizeof(int))) {
        std::cout << level_length << std::endl;
    }

    if(file.read(reinterpret_cast< char* >(&level_width), sizeof(int))) {
        std::cout << level_width << std::endl;
    }

    std::vector< Magpie::PixelData > pixel_data;
    pixel_data.resize(level_length * level_width);
    if (!file.read(reinterpret_cast< char * >(&pixel_data[0]), pixel_data.size() * sizeof(Magpie::PixelData))) {
		throw std::runtime_error("Failed to read pixel data.");
	}

    if (file.peek() != EOF) {
		std::cerr << "WARNING: trailing data in level file '" << filename << "'" << std::endl;
	}
}