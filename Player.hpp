#pragma once

#include "GameAgent.hpp"
#include "Signalable.hpp"
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

    class Player: public GameAgent, public Signalable {
    public:
        // States specific to the magpie player
        enum class STATE {
            IDLE = 0,
            WALKING,
            STEALING
        };

        Scene::Transform* transform;
        std::vector < HitlistTask > hitlist;
        
        // Runs the lambda functions for each task on the hit list
        // that has not been marked as completed
        void update_hitlist();
        // Returns true if all the tasks in the hitlist are marked as
        // completed
        bool hitlist_complete();

        // Clean print out of the completion status of the player's tasks
        void print_tasks();

        void consume_signal();

        void update(float elapsed);

        void update_state(float elapsed);

        void interact();
    };
}