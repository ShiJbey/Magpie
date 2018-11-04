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
            RIGHT
        };

        // TODO: Place more generally relevant functions here
        virtual void update_state(float elapsed) = 0;
        virtual void interact() = 0;
        
        virtual void walk(float elapsed);
        virtual void turnTo(glm::uvec2 destination);

        glm::vec2 getDirectionVec2();

        void setDestination(glm::uvec2 destination);

    protected:
        // All agents use an int to represent the current state
        // Specific enums may be introduced in chid classes for
        // simplicity
        uint32_t current_state;

        float movespeed = 1.0f;
        DIRECTION orientation;
        glm::vec2 position;
        glm::vec2 current_destination;
        Path path;
    };
}