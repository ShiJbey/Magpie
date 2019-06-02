#pragma once

#include "ai/Navigation.h"

#include <glm/glm.hpp>

namespace Magpie {

    class GameAgent {
    public:

        enum class DIRECTION {
            UP = 0,
            RIGHT,
            DOWN,
            LEFT
        };

        // TODO: Place more generally relevant functions here
        virtual void update_state(float elapsed) = 0;
        virtual void interact() = 0;

        virtual void walk(float elapsed) = 0;
        virtual void turnTo(glm::vec3 destination) = 0;


        uint32_t get_state();
        glm::vec2 getDirectionVec2();
        Path* get_path();
        DIRECTION get_orientation();

        void set_state(uint32_t state);
        virtual void set_path(Path path) = 0;
        virtual void setDestination(glm::vec3 destination);


    protected:
        // All agents use an int to represent the current state
        // Specific enums may be introduced in chid classes for
        // simplicity
        uint32_t current_state = 0;
        float movespeed = 1.5f;
		float accumulate_time = 0.0f;
		glm::vec3 starting_point;
        DIRECTION orientation;
        glm::ivec3 board_position;
        glm::vec3 current_destination;
        bool at_destination;
        Path path;
        Path new_path;
        bool is_new_path = false;
        uint32_t next_destination_index = 0;
    };
}