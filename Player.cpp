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

void Magpie::Player::consume_signal() {

};

void Magpie::Player::update(float elapsed) {
    
};

void Magpie::Player::update_state(float elapsed) {

};

void Magpie::Player::interact() {

};