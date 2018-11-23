#include "AnimationManager.hpp"

#include <iostream>

const glm::vec3 Magpie::AnimationManager::OFF_SCREEN_POS(-10000.0f, -10000.0f, -10000.0f);

void Magpie::AnimationManager::update(float elapsed) {
    current_state->animation_player->update(elapsed);
};

void Magpie::AnimationManager::set_current_state(uint32_t state) {
    if (state < animations.size()) {
        // Check if there is an old animation
        if (current_state == nullptr) {
            AnimationState *next_state = animations[state];
            model_transform = next_state->transform;
            // Set the current animation
            current_state = next_state;
        }
        else {
            AnimationState *next_state = animations[state];
            if (next_state == current_state) {
                return;
            }
            // Correct the transform
            next_state->transform->position = model_transform->position;
            next_state->transform->rotation = model_transform->rotation;

            // Reset the old animation for next use
            current_state->animation_player->reset();
            // Move the old animation off screen
            current_state->transform->position = OFF_SCREEN_POS;
            
            // Set the new current state
            current_state = next_state;
            model_transform = current_state->transform;
        }  
    }
    else {
        printf("ERROR:: Animation State '%d' is out of bounds for animation with '%zd' states", state, animations.size());
    }
};

Scene::Transform ** Magpie::AnimationManager::init(glm::vec3 pos, uint32_t state) {
    set_current_state(state);
    for (auto it = animations.begin(); it != animations.end(); it++) {
        (*it)->transform->position =  OFF_SCREEN_POS;
    }
    model_transform = current_state->transform; 
    model_transform->position = pos;
    return &model_transform;
};

void Magpie::AnimationManager::add_state(AnimationState* state) {
    animations.push_back(state);
    state->transform->position = OFF_SCREEN_POS;
};

Scene::Transform* Magpie::AnimationManager::get_model_transform() {
    return model_transform;
};

Magpie::AnimationState* Magpie::AnimationManager::get_current_animation() {
    return current_state;
};