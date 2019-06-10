#include "GameAgent.hpp"

#include <iostream>


glm::vec2 Magpie::GameAgent::get_facing_direction_as_vec2() {
    switch (this->facing_direction) {
        case DIRECTION::UP:
            return glm::vec2(0, 1);
        case DIRECTION::RIGHT:
            return glm::vec2(1, 0);
        case DIRECTION::DOWN:
            return glm::vec2(0, -1);
        case DIRECTION::LEFT:
            return glm::vec2(-1, 0);
        default:
            std::cerr << "ERROR::GameAgent.get_direction_as_vec2():: Invalid this->facing_direction" << std::endl;
            return glm::vec2(-1, -1);
    }
};

Magpie::GameAgent::DIRECTION Magpie::GameAgent::get_facing_direction() {
    return this->facing_direction;
};

void Magpie::GameAgent::set_facing_direction(Magpie::GameAgent::DIRECTION dir) {
    this->facing_direction = dir;
};

void Magpie::GameAgent::set_destination(glm::vec2 destination) {
    this->current_destination = destination;
};

uint32_t Magpie::GameAgent::get_state() {
    return this->current_state;
};

void Magpie::GameAgent::set_state(uint32_t state) {
    this->current_state = state;
};

Magpie::Path* Magpie::GameAgent::get_path() {
    return &(this->path);
};

void Magpie::GameAgent::set_path(Path path) {
    this->path = path;
};

void Magpie::GameAgent::append_path(Path new_path) {

    // Do nothing for empty path
    if (new_path.get_path().size() == 0) {
        return;
    }

    // The GameAgent has either finished their current path
    // or has not started its current path
    if (this->path_destination_index >= this->path.get_path().size()
        || this->path_destination_index == 0) {
        this->path = new_path;
        this->path_destination_index = 0;
        return;
    }

    // The GameAgent has been given a path and the current path is not complete
    else {
        // Remove all locations in the path vector after the current destination
        // Append this path to the end of the old path and let the magpie continue
        // as normal
        std::vector<glm::vec2> modified_path = this->path.get_path();
        std::vector<glm::vec2> path_to_add = new_path.get_path();

        // Ignore if the new path is taking you to the same place
        // as your current path
        if (path_to_add.back() == this->get_path()->get_path().back()) {
            return;
        }

        // Erase all locations after the next destination
        modified_path.erase(modified_path.begin() + path_destination_index, modified_path.end());

        // Append all the locations in the given path
        for(auto &pos : path_to_add) {
            modified_path.push_back(pos);
        }

        // Set the path to the newly modified one
        this->path.set_path(modified_path);
    }
};

Magpie::GameAgent::DIRECTION Magpie::GameAgent::direction_toward(glm::vec2 pos, glm::vec2 dest) {

    // Create a vector from the position to the destination
    glm::vec2 difference = dest - pos;

    // Return RIGHT if diff.x is greater than or equal to zero
    // and the difference in the y direction is less than the
    // difference in the x
    if (difference.x >= 0 && difference.x >= std::abs(difference.y)) {
        return DIRECTION::RIGHT;
    }

    // Return LEFT if diff.x is less than zero
    // and the difference in the y direction is less than the
    // difference in the x
    if (difference.x < 0 && std::abs(difference.x) >= std::abs(difference.y)) {
        return DIRECTION::LEFT;
    }

    // Return UP if diff.y is greater than or equal to zero
    // and the difference in the y direction is greater than
    // the difference in the x direction
    if (difference.y >= 0 && difference.y >= std::abs(difference.x)) {
        return DIRECTION::UP;
    }

    // Return DOWN by default
    return DIRECTION::DOWN;
};
