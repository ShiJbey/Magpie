#include "MagpieLevel.hpp"

Magpie::MagpieLevel::MagpieLevel(uint32_t _rows, uint32_t _cols) {
    this->rows = _rows;
    this->cols = _cols;

    // Set up the movement matrix
    for (uint32_t i = 0; i < rows; i++) {
        std::vector< bool > row;
        for (uint32_t j = 0; j < cols; j++) {
            row.push_back(false);
        }
        movement_matrix.push_back(row);
    }

    // Set up matrix with interactable positions
    for (uint32_t i = 0; i < rows; i++) {
        std::vector< bool > row;
        for (uint32_t j = 0; j < cols; j++) {
            row.push_back(false);
        }
        interaction_map.push_back(row);
    }
}

glm::uvec2 Magpie::MagpieLevel::floor_tile_coord(glm::vec3 isect) {
    float r = std::floor(isect.x + 0.5f);
    float c = std::floor(isect.y + 0.5f);
    bool negative = (r < 0.0f || c < 0.0f);
    bool outOfRange = (r >= rows || c >= cols);
    if (negative || outOfRange) {
        //click is negative and impossible or is greater than dims of row and cols of given map
        return glm::uvec2(-1, -1);
    }
    return glm::uvec2(r, c);
}


bool Magpie::MagpieLevel::can_move_to(uint32_t row, uint32_t col) {
    if (row < movement_matrix.size()) {
        if (col < movement_matrix[row].size()) {
            return this->movement_matrix[row][col];
        }
    }
    return false;    
}

void Magpie::MagpieLevel::handle_click() {
            
}