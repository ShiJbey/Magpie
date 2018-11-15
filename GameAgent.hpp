#pragma once

#include "Navigation.h"

#include <glm/glm.hpp>

namespace Magpie {
    
    class GameAgent {
    public:

        enum class DIRECTION {
            DOWN = 0,
            LEFT,
            UP,
            RIGHT,
            UP_RIGHT,
            UP_LEFT,
            DOWN_RIGHT,
            DOWN_LEFT
        };

        // TODO: Place more generally relevant functions here
        virtual void update_state(float elapsed) = 0;
        virtual void interact() = 0;
        
        virtual void walk(float elapsed) = 0;
        virtual void turnTo(glm::uvec2 destination);
        uint32_t get_state();
        void set_state(uint32_t state);

        glm::vec2 getDirectionVec2();

        virtual void setDestination(glm::uvec2 destination);

    protected:
        // All agents use an int to represent the current state
        // Specific enums may be introduced in chid classes for
        // simplicity
        uint32_t current_state;

        float movespeed = 1.0f;
        DIRECTION orientation;
        glm::uvec2 board_position;
        glm::vec2 current_destination;
        bool at_destination;
        Path path;
        glm::vec3 velocity;
    };
}