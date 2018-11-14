#include "Player.hpp"


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
            //printf("DESTINATION REACHED\n");
            current_destination = path.next();
            //printf("NEXT DESTINATION: ( %f, %f)\n", current_destination.x, current_destination.y);
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
    animation_manager.update(elapsed);

    if (current_state == (uint32_t)Player::STATE::WALKING) {
        walk(elapsed);
    }
};

void Magpie::Player::update_state(float elapsed) {

};

void Magpie::Player::interact() {

};

void Magpie::Player::set_position(glm::vec3 position) {
    if (transform != nullptr) {
        (*transform)->position = position;
    }
    board_position = glm::vec2(position);
};

glm::vec3 Magpie::Player::get_position() {
    if (transform != nullptr) {
        return (*transform)->position;
    }
    return glm::vec3(-1.0f, -1.0f, 0.0f);
};

Magpie::Player::Player() {
    // Default to position off the map
    current_destination = glm::vec2(0, 0);
    orientation = DIRECTION::LEFT;
    movespeed = 2.0f;
};

void Magpie::Player::set_state(uint32_t state) {
    GameAgent::set_state(state);
    animation_manager.set_current_state(state);
};

Scene::Transform** Magpie::Player::get_transform() {
    return transform;
};

void Magpie::Player::set_model_orientation(GameAgent::DIRECTION dir) {
    switch(dir) {
        case DIRECTION::RIGHT :
            std::cout << "DEBUG:: Facing right" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case DIRECTION::LEFT :
            std::cout << "DEBUG:: Facing left" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case DIRECTION::UP :
            std::cout << "DEBUG:: right up" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case DIRECTION::DOWN:
            (*transform)->rotation = original_rotation;
            break;
    }
};

void Magpie::Player::set_transform(Scene::Transform** transform) {
    this->transform = transform;
    original_rotation = (*transform)->rotation;
};

Magpie::AnimationManager* Magpie::Player::get_animation_manager() {
    return &animation_manager;
};

void Magpie::Player::set_score(uint32_t score) {
    this->score = score;
    printf("DEBUG:: Player score is (%d).", this->score);
};

uint32_t Magpie::Player::get_score() {
    return this->score;
};

