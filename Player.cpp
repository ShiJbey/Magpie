#include "Player.hpp"


Magpie::Player::Player() {
    current_destination = glm::vec2(-0.0f, -0.0f);
    orientation = DIRECTION::LEFT;
    movespeed = 2.0f;
};

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
    if (current_destination == glm::vec2(-1.0f, -1.0f)) {
        current_destination = glm::vec2(get_position().x, get_position().y);
    }

    Player::set_position((*transform)->position + (velocity * elapsed));
    float distance_to_destination = glm::length(glm::vec3((float)current_destination.x, (float)current_destination.y, 0.0f) - get_position());
    
    if (distance_to_destination <= 0.01f || current_destination == glm::vec2(-1.0f, -1.0f || distance_to_destination > 2.0f)){//|| glm::dot(vector_to, getDirectionVec2()) < 0) {
        if (path.isEmpty()) {
            printf("DEBUG:: DONE PATH\n");
            Player::set_state((uint32_t)Player::STATE::IDLE);
            velocity = glm::vec3(0.0f, 0.0f, 0.0f);
            return;
        } else {
            
            Player::set_position(glm::vec3(current_destination, 0.0f));
            printf("DESTINATION REACHED\n");
            glm::vec2 next_destination = path.next();
            printf("NEXT DESTINATION: ( %f, %f)\n", next_destination.x, next_destination.y);
            turnTo(next_destination);
            current_destination = next_destination;
            glm::vec3 vec_to_next_destination = glm::vec3((float)current_destination.x, (float)current_destination.y, 0.0f) - get_position();
            if (glm::length(vec_to_next_destination) != 0.0f) {
                velocity = glm::normalize(vec_to_next_destination) * 2.0f;
            } else {
                velocity = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            set_model_orientation(orientation);
        }
    }
};

void Magpie::Player::consume_signal() {

};

void Magpie::Player::update(float elapsed) {
    animation_manager->update(elapsed);
    
    if (current_state == (uint32_t)Player::STATE::WALKING) {
        walk(elapsed);
    }

    if (current_state == (uint32_t)Player::STATE::STEALING && animation_manager->get_current_animation()->animation_player->done()) {
        set_state((uint32_t)Player::STATE::IDLE);
        set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
    }
};

void Magpie::Player::update_state(float elapsed) {

};

void Magpie::Player::interact() {

};

void Magpie::Player::set_position(glm::vec3 position) {
    Magpie::GameCharacter::set_position(position);
    board_position = glm::uvec2(position);
};




void Magpie::Player::set_state(uint32_t state) {
    GameAgent::set_state(state);
    animation_manager->set_current_state(state);
};









void Magpie::Player::set_score(uint32_t score) {
    this->score = score;
    printf("DEBUG:: Player score is (%d).", this->score);
};

uint32_t Magpie::Player::get_score() {
    return this->score;
};

