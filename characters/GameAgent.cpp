#include "GameAgent.hpp"

#include <iostream>


glm::vec2 Magpie::GameAgent::getDirectionVec2() {
    switch (orientation) {
        case DIRECTION::UP:
            return glm::vec2(0, 1);
        case DIRECTION::RIGHT:
            return glm::vec2(1, 0);
        case DIRECTION::DOWN:
            return glm::vec2(0, -1);
        case DIRECTION::LEFT:
            return glm::vec2(-1, 0);
        default:
            std::cerr << "ERROR::GameAgent.getDirectionVec2():: Invalid Orientation" << std::endl;
            return glm::vec2(0, -1);
    }
};

Magpie::GameAgent::DIRECTION Magpie::GameAgent::get_orientation() {
    return this->orientation;
};

void Magpie::GameAgent::setDestination(glm::vec3 destination) {
    current_destination = glm::vec3(this->path.top(), 0.0f);
    turnTo(current_destination);
};

uint32_t Magpie::GameAgent::get_state() {
    return current_state;
};

void Magpie::GameAgent::set_state(uint32_t state) {
    current_state = state;
};

Magpie::Path* Magpie::GameAgent::get_path() {
    return &path;
};
