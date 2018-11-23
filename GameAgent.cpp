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
        //case DIRECTION::UP_RIGHT:
        //    return glm::vec2(1, 1);
        //case DIRECTION::UP_LEFT:
        //    return glm::vec2(-1, 1);
        //case DIRECTION::DOWN_RIGHT:
        //    return glm::vec2(1, -1);
        //case DIRECTION::DOWN_LEFT:
        //    return glm::vec2(-1, -1);
        default:
            return glm::vec2(0, 0);
    }
};

Magpie::GameAgent::DIRECTION Magpie::GameAgent::get_orientation() {
    return this->orientation;
};

void Magpie::GameAgent::setDestination(glm::vec3 destination) {
   /// this->path = Magpie::Navigation::getInstance().findPath(board_position, destination);
    current_destination = glm::vec3(this->path.top(), 0.0f);
    turnTo(current_destination);
};

uint32_t Magpie::GameAgent::get_state() {
    return current_state;
};

void Magpie::GameAgent::set_state(uint32_t state) {
    current_state = state;
};

void Magpie::GameAgent::turnTo(glm::vec3 destination) {
    //std::cout << "TURNING" << std::endl;
//
    //// Face right
    //if (destination.x > board_position.x) {
    //    if (destination.y > board_position.y) {
    //        orientation = DIRECTION::UP_RIGHT;
    //    }
    //    else if (destination.y < board_position.y) {
    //        orientation = DIRECTION::DOWN_RIGHT;
    //    }
    //    else {
    //        orientation = DIRECTION::RIGHT;
    //    }
    //}
    //// Face Left
    //else if (destination.x < board_position.x) {
    //    if (destination.y > board_position.y) {
    //        orientation = DIRECTION::UP_LEFT;
    //    }
    //    else if (destination.y < board_position.y) {
    //        orientation = DIRECTION::DOWN_LEFT;
    //    }
    //    else {
    //        orientation = DIRECTION::LEFT;
    //    }
    //}
    //else {
    //    if (destination.y > board_position.y) {
    //        orientation = DIRECTION::UP;
    //    }
    //    else {
    //        orientation = DIRECTION::DOWN;
    //    }
    //}
};

Magpie::Path* Magpie::GameAgent::get_path() {
    return &path;
};

void Magpie::GameAgent::set_path(Magpie::Path path) {
    this->path = path;

    this->next_destination_index = 0;
    printf("Reseting next destination index to %d\n", next_destination_index);
};