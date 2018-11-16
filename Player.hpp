#pragma once

#include "GameAgent.hpp"
#include "Signalable.hpp"
#include "AnimationManager.hpp"
#include "GameCharacter.hpp"

#include <glm/glm.hpp>

#include <string>
#include <iostream>


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

    class Player: public GameCharacter, public GameAgent, public Signalable {
    public:
        
        // States specific to the magpie player
        enum class STATE {
            IDLE = 0U,
            WALKING,
            STEALING
        };

        Player();

        void load_character_model();
        
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
        
        // SETTERS
        void set_position(glm::vec3 position);
        void set_state(uint32_t state);
        void set_score(uint32_t score);
        
        // GETTERS
        uint32_t get_score();
        
    protected:

        // Maintains the total value of all items stolen
        // by the player
        uint32_t score;

        // NOTE:: This is old code, but it may be used later
        //        when the game is in a more polished state
        std::vector < HitlistTask > hitlist;
    
    };
}