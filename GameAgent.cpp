#include "GameAgent.hpp"

#include <iostream>

glm::vec2 Magpie::GameAgent::getDirectionVec2() {
    switch (orientation) {
        case DIRECTION::UP:
            return glm::vec2(0, 1);
        case DIRECTION::RIGHT:
            return glm::vec2(-1, 0);
        case DIRECTION::DOWN:
            return glm::vec2(0, -1);
        case DIRECTION::LEFT:
            return glm::vec2(1, 0);
        default:
            return glm::vec2(0, 0);
    }
};

void Magpie::GameAgent::setDestination(glm::uvec2 destination) {
    this->path = Magpie::Navigation::getInstance().findPath(board_position, destination);
    current_destination = this->path.top();
    turnTo(current_destination);
};

uint32_t Magpie::GameAgent::get_state() {
    return current_state;
}

void Magpie::GameAgent::set_state(uint32_t state) {
    current_state = state;
}

void Magpie::GameAgent::turnTo(glm::uvec2 destination) {
    //std::cout << "TURN TO: " << destination.x << "," << destination.y << std::endl;
    int x_direction = Magpie::sgn(destination.x - board_position.x);
    int y_direction = Magpie::sgn(destination.y - board_position.y);

    if (x_direction == 0) {
        if (y_direction == 1) {
            orientation = DIRECTION::UP;
        } else {
            orientation = DIRECTION::DOWN;
        }
    }

    if (y_direction == 0) {
        if (x_direction == 1) {
            orientation = DIRECTION::LEFT;
        } else {
            orientation = DIRECTION::RIGHT;
        }
    }
};