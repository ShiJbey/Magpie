#pragma once

#include "ai/Navigation.h"

#include <glm/glm.hpp>

namespace Magpie {

    /**
     * GameAgent is the abstract parent class that
     * represents all the Characters in the game
     */
    class GameAgent {
    public:

        // Directions for GameAgents to face
        enum class DIRECTION {
            UP = 0,
            RIGHT,
            DOWN,
            LEFT
        };

        // Update things like the current state
        virtual void update(float elapsed) = 0;
        // Moves the GameAgent toward its next destination
        virtual void walk(float elapsed) = 0;
        // Returns what direction a given destination is from a given position
        static DIRECTION direction_toward(glm::vec2 pos, glm::vec2 dest);

        // Get and set the state of the GameAgent
        uint32_t get_state();
        void set_state(uint32_t state);

        // Gets and sets the current path of the GameAgent
        std::vector< glm::vec2 > get_path();
        virtual void set_path(std::vector< glm::vec2 > path);
        // Appends a path to the existing path
        // This is used when changing the final destination mid path
        virtual void append_path(std::vector< glm::vec2 > path_to_add);

        // Get the direction that the Game Agent is facing
        void set_facing_direction(DIRECTION dir);
        DIRECTION get_facing_direction();

        // Get the facing direction as a vec2
        glm::vec2 get_facing_direction_as_vec2();

        // Sets the destination
        virtual void set_destination(glm::vec2 destination);
        glm::vec2 get_current_destination() { return this->current_destination; }


    protected:
        // All agents use an unsigned int to represent the current state
        // Specific enums may be introduced in chid classes for
        // simplicity and more specific configurations
        uint32_t current_state = 0;
        // How fast can this GameAgent walk
        float movespeed = 1.5f;
		// What position is the GameAgent coming from
		glm::vec2 starting_point;
        // What direction is the GameAgent facing
        DIRECTION facing_direction;
        // What position is the GamAgent moving towards
        glm::vec2 current_destination;
        glm::vec2 previous_destination;
        // Has the GameAgent reached its destination
        bool destination_reached;
        // Path that the GameAgent is following
        std::vector< glm::vec2 > path;
        // Index of the current destination within the path
        uint32_t path_destination_index = 0;

    };
}