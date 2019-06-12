#include "MagpieLevel.hpp"

#include <algorithm>

Magpie::MagpieLevel::MagpieLevel(uint32_t width, uint32_t length) {

    this->width = width;
    this->length = length;

    // Instantiate the navigation grid, wall grid, and floor grids
    for (uint32_t row = 0; row < length; ++row) {
        std::vector< bool > nav_row;
        std::vector< Wall* > wall_row;
        std::vector< FloorTile* > floor_row;
        for (uint32_t col = 0; col < width; ++col) {
            nav_row.push_back(false);
            wall_row.push_back(nullptr);
            floor_row.push_back(nullptr);
        }
        nav_grid.push_back(nav_row);
        wall_grid.push_back(wall_row);
        floor_grid.push_back(floor_row);
    }
};

Magpie::MagpieLevel::~MagpieLevel() {
    delete this->green_card;
    delete this->pink_card;
    delete this->master_key;
    delete this->dogTreatPickUp;
    delete this->cardboard_box;
    delete this->back_exit;
};


glm::ivec2 Magpie::MagpieLevel::floor_tile_coord(glm::vec3 isect) {
    float x = std::floor(isect.x + 0.5f);
    float y = std::floor(isect.y + 0.5f);
    if (is_within_bounds(x, y)) {
        //click is negative and impossible or is greater than dims of row and cols of given map
        return glm::ivec2(-1, -1);
    }
    return glm::ivec2(x, y);
};

bool Magpie::MagpieLevel::is_within_bounds(uint32_t x, uint32_t y) {
    return is_within_bounds((float)x, (float)y);
}

bool Magpie::MagpieLevel::is_within_bounds(float x, float y) {
    float x_shifted = std::floor(x + 0.5f);
    float y_shifted = std::floor(y + 0.5f);
    bool negative = (x_shifted < 0.0f || y_shifted < 0.0f);
    bool outOfRange = (x >= this->width || y >= this->length);
    return !(negative || outOfRange);
}

bool Magpie::MagpieLevel::can_move_to(uint32_t current_room, float x, float y) {
    if (is_within_bounds(x, y)) {
        return nav_grid[(uint32_t)y][(uint32_t)x];
            //&& floor_grid[(uint32_t)y][(uint32_t)x]->room_number == current_room;
    }
    return false;
};

bool Magpie::MagpieLevel::can_move_to(uint32_t current_room, uint32_t x, uint32_t y) {
    return can_move_to(current_room, (float)x, (float)y);
};

bool Magpie::MagpieLevel::is_wall(float x, float y) {
    if (is_within_bounds(x, y)) {
        Wall* wall = wall_grid[(uint32_t)y][(uint32_t)x];
        return !(wall == nullptr);
    }
    return false;
};

bool Magpie::MagpieLevel::is_wall(uint32_t x, uint32_t y) {
    return is_wall((float)x, (float)y);
};

Magpie::Wall* Magpie::MagpieLevel::get_wall(float x, float y) {
    if (is_within_bounds(x, y)) {
        return wall_grid[(uint32_t)y][(uint32_t)x];
    }
    return nullptr;
};

Magpie::Wall* Magpie::MagpieLevel::get_wall(uint32_t x, uint32_t y) {
    return get_wall((float)x, (float)y);
};

void Magpie::MagpieLevel::set_wall(Wall* wall, uint32_t x, uint32_t y) {
    set_wall(wall, (float)x, (float)y);
};

void Magpie::MagpieLevel::set_wall(Wall* wall, float x, float y) {
    if (is_within_bounds(x, y)) {
        wall_grid[(uint32_t)y][(uint32_t)x] = wall;
    }
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

uint32_t Magpie::MagpieLevel::get_tile_room_number(uint32_t x, uint32_t y) {
    return get_tile_room_number((float)x, (float)y);
};

uint32_t Magpie::MagpieLevel::get_tile_room_number(float x, float y) {
    if (is_within_bounds(x, y)) {
        if (floor_grid[(uint32_t)y][(uint32_t)x]) {
            return floor_grid[(uint32_t)y][(uint32_t)x]->room_number;
        }
    }
    return -1U;
};


void Magpie::MagpieLevel::set_player_start_position(glm::vec3 start_position) {
    this->player_start_position = start_position;
};

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

std::map< uint32_t, std::map< uint32_t, std::pair<glm::vec3, Magpie::GameAgent::DIRECTION> > >* Magpie::MagpieLevel::get_guard_start_positions() {
    return &(this->guard_start_positions);
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

void Magpie::MagpieLevel::add_displaycase(DisplayCase* displaycase) {
    displaycases.push_back(displaycase);
};

void Magpie::MagpieLevel::set_nav_grid_position(uint32_t x, uint32_t y, bool can_walk) {
    if (is_within_bounds(x, y)) {
        this->nav_grid[y][x] = can_walk;
    }
};

