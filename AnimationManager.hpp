#pragma once

#include "Scene.hpp"
#include "TransformAnimation.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace Magpie {

    class AnimationState {
    public:
        AnimationState(Scene::Transform* _transform, TransformAnimationPlayer* _animation_player) {
            this->transform = _transform;
            this->animation_player = _animation_player;
        }

        Scene::Transform *transform;  // Pointer to the top level transform in the model
        TransformAnimationPlayer* animation_player;
    };

    //Encapulates all the logic for playing animations and swapping out animated models
    class AnimationManager {
    public:
        // Updates the current animation
        void update(float elapsed);

        // Swaps out the current animations
        void set_current_state(uint32_t state);

        // Positions the animation model for the given state
        // and returns a pointer to the transform of the
        // current animation;
        Scene::Transform** init(glm::vec3 pos, uint32_t state);

        // Add state to the animation manager and move it off screen
        void add_state(AnimationState* state);

        // Get pointer to top level transform of the current animation
        Scene::Transform* get_model_transform();

        // Pointer to current animation
        AnimationState* get_current_animation();

        // Position off screen where we move un-used meshes
        static const glm::vec3 OFF_SCREEN_POS;

    protected:
        // Pointer to top level transform of the current animation
        Scene::Transform *model_transform = nullptr;
        AnimationState *current_state = nullptr;
        std::vector< AnimationState* > animations; 
    };
}