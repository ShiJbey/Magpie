#include "MagpieLevel.hpp"



Magpie::MagpieLevel::MagpieLevel(uint32_t width, uint32_t length) {
    this->width = width;
    this->length = length;
    
    // Set up the movement matrix
    for (uint32_t x = 0; x < width; x++) {
        std::vector< bool > col;
        for (uint32_t y = 0; y < length; y++) {
            col.push_back(false);
        }
        movement_matrix.push_back(col);
    }
}

glm::uvec2 Magpie::MagpieLevel::floor_tile_coord(glm::vec3 isect) {
    float x = std::floor(isect.x + 0.5f);
    float y = std::floor(isect.y + 0.5f);
    bool negative = (x < 0.0f || y < 0.0f);
    bool outOfRange = (x >= this->width || y >= this->length);
    if (negative || outOfRange) {
        //click is negative and impossible or is greater than dims of row and cols of given map
        return glm::uvec2(-1, -1);
    }
    return glm::uvec2(x, y);
}


bool Magpie::MagpieLevel::can_move_to(uint32_t row, uint32_t col) {
    if (row < movement_matrix.size()) {
        if (col < movement_matrix[row].size()) {
            return this->movement_matrix[row][col];
        }
    }
    return false;    
};

uint32_t Magpie::MagpieLevel::get_length() { 
    return this->length;
};

uint32_t Magpie::MagpieLevel::get_width() { 
    return this->width;
};

glm::uvec2 Magpie::MagpieLevel::get_dimensions() {
    return glm::uvec2(width, length);
};

std::vector< std::vector< bool > >* Magpie::MagpieLevel::get_movement_matrix() { 
    return &movement_matrix;
};

void Magpie::MagpieLevel::set_movement_matrix_position(uint32_t x, uint32_t y, bool can_walk) {
    this->movement_matrix[x][y] = can_walk;
};

std::map< uint32_t, std::vector< Magpie::Painting > >* Magpie::MagpieLevel::get_paintings() {
    return &paintings;
};

std::map< uint32_t, std::vector< Magpie::Gem > >* Magpie::MagpieLevel::get_gems() {
    return &gems;
};

void Magpie::MagpieLevel::add_painting(uint32_t room_number, Magpie::Painting painting) {
    auto it = paintings.find(room_number);
    if (it == paintings.end()) {
        paintings.insert({room_number, {painting}}); 
    }
    else{
        paintings[room_number].push_back(painting);
    }
};

void Magpie::MagpieLevel::add_gem(uint32_t room_number, Gem gem) {
    auto it = paintings.find(room_number);
    if (it == paintings.end()) {
        gems.insert({room_number, {gem}}); 
    }
    else{
        gems[room_number].push_back(gem);
    }
};

// Adds a position to a guards path
void Magpie::MagpieLevel::add_guard_path_position(uint32_t room_number, uint32_t guard_number, uint32_t x, uint32_t y) {
    auto room_number_iter = guard_paths_by_room.find(room_number);
    if (room_number_iter != guard_paths_by_room.end()) {
        auto guard_number_iter = room_number_iter->second.find(guard_number);
        if (guard_number_iter != room_number_iter->second.end()) {
            guard_number_iter->second.push_back(glm::vec2(x, y));
        }
        else {
            guard_paths_by_room[room_number].insert({ guard_number, {glm::vec2(x, y)}});
        }
    }
    else {
        guard_paths_by_room.insert({ room_number, std::map<uint32_t, std::vector<glm::vec2>>()});
        guard_paths_by_room[room_number].insert(std::make_pair( guard_number, std::vector<glm::vec2>(1, glm::vec2(x, y))));
    }
};

void Magpie::MagpieLevel::handle_click() {
            
};