#pragma once

#include "GameAgent.hpp"
#include "Signalable.hpp"
#include "AnimationManager.hpp"
#include <string>
#include <iostream>
#include <glm/glm.hpp>

namespace Magpie {

    struct HitlistTask {
        bool completed = false;
        std::string description;
        //std::function< bool(Player *) > completion_check;

        std::string to_string() {
            if (completed) {
                return "[ ] Task: " + description;
            } else {
                return "[x] Task: " + description;
            }
        };
    };

    class Player: public GameAgent, public Signalable {
    public:
        
        // States specific to the magpie player
        enum class STATE {
            IDLE = 0,
            WALKING,
            STEALING
        };

        Player();
        
        // Runs the lambda functions for each task on the hit list
        // that has not been marked as completed
        void update_hitlist();
        // Returns true if all the tasks in the hitlist are marked as
        // completed
        bool hitlist_complete();

        // Clean print out of the completion status of the player's tasks
        void print_tasks();

        void walk(float elapsed);

        void consume_signal();

        void update(float elapsed);

        void update_state(float elapsed);

        void interact();

        void set_position(glm::vec3 position) {
            if (transform != nullptr) {
                (*transform)->position = position;
            }
            board_position = glm::vec2(position);
            //board_position.x = std::round(board_position.x);
            //board_position.y = std::round(board_position.y);
        }

        glm::vec3 get_position() {
            if (transform != nullptr) {
                return (*transform)->position;
            }
            return glm::vec3(-1.0f, -1.0f, 0.0f);
        }

        void set_state(uint32_t state) {
            GameAgent::set_state(state);
            animation_manager.set_current_state(state);
        }

        Scene::Transform** get_transform() {
            return transform;
        };

        void set_model_orientation(DIRECTION dir) {
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

        void set_transform(Scene::Transform** transform) {
            this->transform = transform;
            original_rotation = (*transform)->rotation;
        };

        AnimationManager* get_animation_manager() {
            return &animation_manager;
        }

    protected:
        // This points to another transform pointer
        // handled by the animation manager
        Scene::Transform** transform;
        std::vector < HitlistTask > hitlist;
        AnimationManager animation_manager;
        glm::quat original_rotation;
    };
}