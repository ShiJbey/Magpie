#include "Guard.hpp"

#include <iostream>

void Magpie::Guard::walk(float elapsed) {
    float distance = elapsed * movespeed;

    glm::vec2 vector_to =  current_destination - board_position;

//    std::cout << "Walking to " << cur_destination.x << "," << cur_destination.y << "from" << position.x << ","
//              << position.y << "Direction" << orientation << ":" << getDirectionVec2().x << "," << getDirectionVec2().y << std::endl;

    if (glm::length(vector_to) < distance || glm::dot(vector_to, getDirectionVec2()) < 0) {

        board_position = current_destination;
        if (path.isEmpty()) {
            std::cout << "EMPTY" << std::endl;
            if (current_state != (uint32_t)Guard::STATE::CHASING) current_state = (uint32_t)Guard::STATE::IDLE;
        } else {
            std::cout << "NEXT" << std::endl;
            current_destination = path.next();
            turnTo(current_destination);
        }
    } else {
        board_position += getDirectionVec2() * distance;
    }
};

void Magpie::Guard::consume_signal() {

};

void Magpie::Guard::update(float elapsed) {
    
};

void Magpie::Guard::update_state(float elapsed) {

};

void Magpie::Guard::interact() {

};