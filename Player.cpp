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

    //printf("Current Destination: ( %f, %f)\n", current_destination.x, current_destination.y);
    //printf("Current Board Position: ( %f, %f)\n", board_position.x, board_position.y);
    //printf("Current Trans Position: ( %f, %f)\n", (*transform)->position.x, (*transform)->position.y);
    //turnTo(current_destination);
    
    float distance = elapsed * movespeed;
    glm::vec2 displacement = getDirectionVec2() * distance;

    glm::vec2 vector_to =  current_destination - glm::vec2(board_position);

    if (glm::length(vector_to) < distance || glm::dot(vector_to, getDirectionVec2()) < 0) {
        if (path.isEmpty()) {
            Player::set_state((uint32_t)Player::STATE::IDLE);
            return;
        } else {
            
            Player::set_position(glm::vec3(current_destination, 0.0f));
            printf("DESTINATION REACHED\n");
            current_destination = path.next();
            printf("NEXT DESTINATION: ( %f, %f)\n", current_destination.x, current_destination.y);
            turnTo(current_destination);
            set_model_orientation(orientation);
            
            
        }
    } else {
        Player::set_position((*transform)->position + glm::vec3(displacement, 0.0f));
    }
};

void Magpie::Player::consume_signal() {

};

void Magpie::Player::update(float elapsed) {
    ///board_position = glm::vec2((*transform)->position);
    animation_manager.update(elapsed);
    //if (current_destination == board_position && current_state != (uint32_t)Player::STATE::IDLE) {
       // current_state = (uint32_t)Player::STATE::IDLE;
    //}

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
    current_destination = glm::vec2(0, 0);
    orientation = DIRECTION::LEFT;
    movespeed = 2.0f;
};