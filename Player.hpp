#pragma once

#include "GameAgent.hpp"
#include "Signalable.hpp"
#include "AnimationManager.hpp"
#include "AnimatedModel.hpp"

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

    class Player: public AnimatedModel, public GameAgent, public Signalable {
    public:
        // This is incremented each time we create a new player
        static uint32_t instance_count;

        // States specific to the magpie player
        enum class STATE {
            IDLE = 0,
            WALKING,
            STEALING,
            PICKING,
            BREAKING,
            DISGUISE_IDLE,
            DISGUISE_WALK
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

        virtual void setDestination(glm::vec3 destination);
        virtual void turnTo(glm::vec3 destination);

        // Loads Magpie model data specifically
        virtual Scene::Transform* load_model(Scene& scene, const ModelData* model_data, std::string model_name,
            std::function< Scene::Object*(Scene &, Scene::Transform *, std::string const &) > const &on_object);

        virtual std::vector< std::string > convert_animation_names(const TransformAnimation* tanim, std::string model_name);

        // SETTERS
        void set_position(glm::vec3 position);
        void set_state(uint32_t state);
        void set_score(uint32_t score);
        void set_current_room(uint32_t room_number);
        virtual void set_model_orientation(uint32_t dir);

        // GETTERS
        uint32_t get_score();
        uint32_t get_current_room();

        // Keys placed in level
        bool has_green_card = false;
        bool has_pink_card = false;
        bool has_master_key = false;

        bool is_disguised();

        glm::vec3 final_destination = glm::vec3(-1, -1, -1);

        bool game_over = false;

    protected:

        // Maintains the total value of all items stolen
        // by the player
        uint32_t score;

        // NOTE:: This is old code, but it may be used later
        //        when the game is in a more polished state
        std::vector < HitlistTask > hitlist;

        // Used to force the player to walk through doors
        uint32_t current_room = 0;
    };
}