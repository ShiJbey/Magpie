#include "load_level.hpp"
#include <iostream>
#include <fstream>
#include <vector>

uint8_t Magpie::PixelData::COLLIDABLE_MESH_MASK = 0b11110000;
uint8_t Magpie::PixelData::NONCOLLIDABLE_MESH_MASK = 0b00001111;
uint8_t Magpie::PixelData::ROOM_NUMBER_MASK = 0b11110000;
uint8_t Magpie::PixelData::GUARD_PATH_MASK = 0b00001000;
uint8_t Magpie::PixelData::OBJECT_ID_MASK = 0b11110000;
uint8_t Magpie::PixelData::INTERACTION_FUNC_ID_MASK = 0b00001100;
uint8_t Magpie::PixelData::INTERACTION_FLAG_MASK = 0b00000011;

uint8_t Magpie::PixelData::COLLIDABLE_MESH_OFFSET = 4;
uint8_t Magpie::PixelData::NONCOLLIDABLE_MESH_OFFSET = 0;
uint8_t Magpie::PixelData::ROOM_NUMBER_OFFSET = 4;
uint8_t Magpie::PixelData::GUARD_PATH_OFFSET = 3;
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
    uint8_t collidable_mesh_id = (red_channel_data & COLLIDABLE_MESH_MASK) >> COLLIDABLE_MESH_OFFSET;
    uint8_t noncollidable_mesh_id = (red_channel_data & NONCOLLIDABLE_MESH_MASK) >> NONCOLLIDABLE_MESH_OFFSET;
    
    if (collidable_mesh_id != 0) {
        return collidable_mesh_id;
    }
    else if (noncollidable_mesh_id != 0) {
        return noncollidable_mesh_id;
    }

    return 0;
}

uint8_t Magpie::PixelData::get_room_number() {
    return (green_channel_data & ROOM_NUMBER_MASK) >> ROOM_NUMBER_OFFSET;
}

uint8_t Magpie::PixelData::is_guard_path() {
    return (green_channel_data & GUARD_PATH_MASK) >> GUARD_PATH_OFFSET;
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

    //std::cout << pixel_data[10].to_string() << std::endl;
}

int main(int argc, char** argv) {
    Magpie::LevelLoader loader;
    loader.load("./scripts/out/test.lvl");

    std::cout << "Hello, World" << std::endl;
    std::cout << sizeof(Magpie::PixelData) << std::endl;
    return 0;
}