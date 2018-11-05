#include "Player.hpp"
#include <iostream>

// Runs the lambda functions for each task on the hit list
// that has not been marked as completed
void Magpie::Player::update_hitlist() {
    for (uint32_t i = 0; i < hitlist.size(); i++) {
        //hitlist[i].completion_check(this);
    }
};

// Returns true if all the tasks in the hitlist are marked as
// completed
bool Magpie::Player::hitlist_complete() {
    for (uint32_t i = 0; i < hitlist.size(); i++) {
        if (!hitlist[i].completed) {
            return false;
        }
    }
    return true;
};

// Clean print out of the completion status of the player's tasks
void Magpie::Player::print_tasks() {
    for (uint32_t i = 0; i < hitlist.size(); i++) {
        std::cout << hitlist[i].to_string() << std::endl;
    }
};

void Magpie::Player::walk(float elapsed) {
    float distance = elapsed * movespeed;

    /*

    if (path.isEmpty()) {
        current_state = (uint32_t)Player::STATE::IDLE;
        animation_manager.set_current_state((uint32_t)Player::STATE::IDLE);
        return;
    } else {
        if (board_position == glm::round(current_destination)) {
            printf("DESTINATION REACHED\n");
            current_destination = path.next();
            printf("NEXT DESTINATION: ( %f, %f)\n", current_destination.x, current_destination.y);
            turnTo(current_destination);
        }
        set_position(glm::vec3(current_destination.x, current_destination.y, 0.0f));

        set_position((*transform)->position + glm::vec3(getDirectionVec2() * distance, 0.0f));
        
        board_position.x = (float)std::floor((*transform)->position.x + 0.5f);
        board_position.y = (float)std::floor((*transform)->position.y + 0.5f);
    }

    */

    glm::vec2 vector_to =  current_destination - glm::vec2(board_position);
    
    if (glm::length(vector_to) < distance || glm::dot(vector_to, getDirectionVec2()) < 0) {

        set_position(glm::vec3(current_destination, 0.0f));
        if (path.isEmpty()) {
            //std::cout << "EMPTY" << std::endl;
        } else {
            //std::cout << "NEXT" << std::endl;
            current_destination = path.next();
            turnTo(current_destination);
        }
    } else {
        set_position((*transform)->position + glm::vec3(getDirectionVec2() * distance, 0.0f));
    }
    
};

void Magpie::Player::consume_signal() {

};

void Magpie::Player::update(float elapsed) {
    ///board_position = glm::vec2((*transform)->position);
    animation_manager.update(elapsed);
    if (current_destination == board_position && current_state != (uint32_t)Player::STATE::IDLE) {
       // current_state = (uint32_t)Player::STATE::IDLE;
    }

    if (current_state == (uint32_t)Player::STATE::WALKING) {
        walk(elapsed);
    }
};

void Magpie::Player::update_state(float elapsed) {

};

void Magpie::Player::interact() {

};

Magpie::Player::Player() {
    // Default to position off the map
    current_destination = glm::vec2(-1, -1);
};