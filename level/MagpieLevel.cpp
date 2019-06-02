#include "MagpieLevel.hpp"
#include "objects/FloorTile.hpp"
#include "objects/Door.hpp"
#include "../characters/GameAgent.hpp"

#include <algorithm>

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

    // Make the floor Matrix
    floor_matrix = new FloorTile**[width];
    for (uint32_t x = 0; x < width; x++) {
        floor_matrix[x] = new FloorTile*[length];
        for (uint32_t y = 0; y < length; y++) {
            floor_matrix[x][y] = nullptr;
        }
    }

    // Make Door Matrix;
    door_matrix = new Door**[width];
    for (uint32_t x = 0; x < width; x++) {
        door_matrix[x] = new Door*[length];
        for (uint32_t y = 0; y < length; y++) {
            door_matrix[x][y] = nullptr;
        }
    }

    // Make the wall Matrix
    wall_matrix = new Wall**[width];
    for (uint32_t x = 0; x < width; x++) {
        wall_matrix[x] = new Wall*[length];
        for (uint32_t y = 0; y < length; y++) {
            wall_matrix[x][y] = nullptr;
        }
    }

};

Magpie::MagpieLevel::~MagpieLevel() {
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0; y < length; y++) {
            FloorTile* tile = floor_matrix[x][y];
            free(tile);
            floor_matrix[width] = new FloorTile*[length];

        }
        FloorTile** col = floor_matrix[x];
        free(col);
    }
    free(floor_matrix);
};

Magpie::FloorTile*** Magpie::MagpieLevel::get_floor_matrix() {
    return floor_matrix;
};


glm::ivec2 Magpie::MagpieLevel::floor_tile_coord(glm::vec3 isect) {
    float x = std::floor(isect.x + 0.5f);
    float y = std::floor(isect.y + 0.5f);
    bool negative = (x < 0.0f || y < 0.0f);
    bool outOfRange = (x >= this->width || y >= this->length);
    if (negative || outOfRange) {
        //click is negative and impossible or is greater than dims of row and cols of given map
        return glm::ivec2(-1, -1);
    }
    return glm::ivec2(x, y);
};

bool Magpie::MagpieLevel::can_move_to(uint32_t current_room, float x, float y) {
    if ((x >= 0 && x < (float)width) && (y >= 0 && y < (float)length)) {
        FloorTile* floor_tile = floor_matrix[(uint32_t)x][(uint32_t)y];
        if (floor_tile == nullptr) {
            return false;
        } else {
            return true;
            //return current_room == floor_tile->room_number;
        }
    }
    return false;
};

bool Magpie::MagpieLevel::can_move_to(uint32_t current_room, uint32_t x, uint32_t y) {
    if (x < width && y < length) {
        FloorTile* floor_tile = floor_matrix[x][y];
        if (floor_tile == nullptr) {
            return false;
        } else {
            return true;
            //return current_room == floor_tile->room_number;
        }
    }
    // OLD CODE
    //if (row < movement_matrix.size()) {
    //    if (col < movement_matrix[row].size()) {
    //        return this->movement_matrix[row][col];
    //    }
    //}
    return false;
};

bool Magpie::MagpieLevel::is_wall(float x, float y) {
    if ((x >= 0 && (uint32_t)x < width) && (y >= 0 && (uint32_t)y < length)) {
        Wall* wall = wall_matrix[(uint32_t)x][(uint32_t)y];
        if (wall == nullptr) {
            return false;
        } else {
            return true;
        }
    }
    return false;
};

bool Magpie::MagpieLevel::is_wall(uint32_t x, uint32_t y) {
    if (x < width && y < length) {
        Wall* wall = wall_matrix[x][y];
        if (wall == nullptr) {
            return false;
        } else {
            return true;
        }
    }
    return false;
};

Magpie::Wall* Magpie::MagpieLevel::get_wall(float x, float y) {
    if ((x >= 0 && (uint32_t)x < width) && (y >= 0 && (uint32_t)y < length)) {
        return wall_matrix[(uint32_t)x][(uint32_t)y];
    }
    return nullptr;
};

Magpie::Wall* Magpie::MagpieLevel::get_wall(uint32_t x, uint32_t y) {
    if (x < width && y < length) {
        return wall_matrix[x][y];
    }
    return nullptr;
};

void Magpie::MagpieLevel::set_wall(Wall* wall, uint32_t x, uint32_t y) {
    if (x < width && y < length) {
        wall_matrix[x][y] = wall;
    }
};

void Magpie::MagpieLevel::set_wall(Wall* wall, float x, float y) {
    if ((x >= 0 && (uint32_t)x < width) && (y >= 0 && (uint32_t)y < length)) {
        wall_matrix[(uint32_t)x][(uint32_t)y] = wall;
    }
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

std::map< uint32_t, std::vector< Magpie::Painting* > >& Magpie::MagpieLevel::get_paintings() {
    return paintings;
};

std::map< uint32_t, std::vector< Magpie::Gem* > >& Magpie::MagpieLevel::get_gems() {
    return gems;
};

void Magpie::MagpieLevel::add_painting(uint32_t room_number, Magpie::Painting *painting) {
    auto it = paintings.find(room_number);
    if (it == paintings.end()) {
        paintings.insert({room_number, {painting}});
    }
    else{
        paintings[room_number].push_back(painting);
    }
};

void Magpie::MagpieLevel::add_gem(uint32_t room_number, Gem* gem) {
    auto it = gems.find(room_number);
    if (it == gems.end()) {
        std::vector< Gem* > gem_vec = { gem };
        gems.insert({room_number, gem_vec});
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

uint32_t Magpie::MagpieLevel::get_tile_room_number(uint32_t x, uint32_t y) {
    if (x < width && y < length) {
        return floor_matrix[x][y]->room_number;
    }
    return -1U;
};

uint32_t Magpie::MagpieLevel::get_tile_room_number(float x, float y) {
    if ((x >= 0.0f && x < width) && (y >= 0.0f && y < length)) {
        return floor_matrix[(uint32_t)x][(uint32_t)y]->room_number;
    }
    return -1U;
};

std::vector< Magpie::Door* >* Magpie::MagpieLevel::get_doors() {
    return &doors;
};

void Magpie::MagpieLevel::set_player_start_position(glm::vec3 start_position) {
    this->player_start_position = start_position;
};

glm::vec3 Magpie::MagpieLevel::get_player_start_position() {
    return this->player_start_position;
}

void Magpie::MagpieLevel::add_guard_start_position(uint32_t room_number, uint32_t guard_number, glm::vec3 start_position, GameAgent::DIRECTION dir) {
    auto room_number_iter = guard_start_positions.find(room_number);
    if (room_number_iter != guard_start_positions.end()) {
        auto guard_number_iter = room_number_iter->second.find(guard_number);
        if (guard_number_iter != room_number_iter->second.end()) {
            std::cout << "WARNING::MagpieLevel:: Attempting to add a duplicate start position" << std::endl;
        }
        else {
            guard_start_positions[room_number].insert(std::make_pair(guard_number, std::make_pair(start_position, dir)));
        }
    }
    else {
        guard_start_positions.insert({room_number, std::map<uint32_t, std::pair<glm::vec3, GameAgent::DIRECTION>>()});
        guard_start_positions[room_number].insert(std::make_pair(guard_number, std::make_pair(start_position, dir)));
    }
};

std::map< uint32_t, std::map< uint32_t, std::pair<glm::vec3, Magpie::GameAgent::DIRECTION> > >& Magpie::MagpieLevel::get_guard_start_positions() {
    return guard_start_positions;
}

std::vector< glm::vec2 > Magpie::MagpieLevel::get_guard_path(uint32_t room_number, uint32_t guard_number) {
    // Check if the level has the given room and guard number;
    auto room_iter = guard_paths_by_room.find(room_number);
    if (room_iter != guard_paths_by_room.end()) {
        auto guard_iter = room_iter->second.find(guard_number);
        if (guard_iter != room_iter->second.end()) {
            // Return the path for this guard
            return guard_iter->second;
        }
    }
    // Return an empty vector
    return std::vector< glm::vec2 >();
};

void Magpie::MagpieLevel::add_potential_location(std::map< uint32_t, std::vector< Scene::Transform* > >* location_map, uint32_t room_number, Scene::Transform* parent_trans) {
    auto room_iter = location_map->find(room_number);
    if (room_iter != location_map->end()) {
        std::vector< Scene::Transform* > transforms = room_iter->second;
        // Add this transform to the vector of potential positions for this room
        if (std::find(transforms.begin(), transforms.end(), parent_trans) == transforms.end()) {
            (*location_map)[room_number].push_back(parent_trans);
        }
        else {
            std::cout << "WARNING::MagpieLevel:: Attempting to add a duplicate item location." << std::endl;
        }
    }
    else {
        location_map->insert({room_number, std::vector< Scene::Transform* >()});
        (*location_map)[room_number].push_back(parent_trans);
    }
};

std::map< uint32_t, std::vector< Scene::Transform* > >* Magpie::MagpieLevel::get_potential_floor_locations() {
    return &potential_floor_locations;
};

std::map< uint32_t, std::vector< Scene::Transform* > >* Magpie::MagpieLevel::get_potential_wall_locations() {
    return &potential_wall_locations;
};

std::map< uint32_t, std::vector< Scene::Transform* > >* Magpie::MagpieLevel::get_potential_pedestal_locations() {
    return &potential_pedestal_locations;
};

std::map< uint32_t, std::vector< Scene::Transform* > >* Magpie::MagpieLevel::get_potential_table_locations() {
    return &potential_table_locations;
};

std::vector< Magpie::DisplayCase* >& Magpie::MagpieLevel::get_displaycases() {
    return displaycases;
};

void Magpie::MagpieLevel::add_displaycase(DisplayCase* displaycase) {
    displaycases.push_back(displaycase);
};

