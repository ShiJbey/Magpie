#pragma once

#include "GameAgent.hpp"
#include "../Signalable.hpp"
#include "../animation/AnimationManager.hpp"
#include "../animation/AnimatedModel.hpp"

#include <glm/glm.hpp>

#include <string>
#include <iostream>


namespace Magpie {

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

        virtual void walk(float elapsed) override;

        virtual void consume_signal() override;

        void update(float elapsed);

        virtual void update_state(float elapsed) override;

        virtual void interact() override;

        virtual void setDestination(glm::vec3 destination) override;
        virtual void turnTo(glm::vec3 destination) override;

        // Loads Magpie model data specifically
        virtual Scene::Transform* load_model(Scene& scene, const ModelData* model_data, std::string model_name,
            std::function< Scene::Object*(Scene &, Scene::Transform *, std::string const &) > const &on_object) override;

        virtual std::vector< std::string > convert_animation_names(const TransformAnimation* tanim, std::string model_name) override;

        // SETTERS
        void set_position(glm::vec3 position);
        void set_state(uint32_t state);
        void set_score(uint32_t score);
        void set_current_room(uint32_t room_number);
        virtual void set_model_orientation(uint32_t dir) override;
        virtual void set_path(Path path) override;

        void reset_treat_cooldown();
        void set_treat_cooldown(float cooldown_time);
        bool can_place_treat();

        // GETTERS
        uint32_t get_score();
        uint32_t get_current_room();

        // Keys placed in level
        bool has_green_card = false;
        bool has_pink_card = false;
        bool has_master_key = false;
        bool has_cardboard_box = false;
        bool has_dog_treats = false;



        bool is_disguised();

        glm::vec3 final_destination = glm::vec3(-1, -1, -1);

        bool game_over = false;
        bool game_won = false;

    protected:

        // Maintains the total value of all items stolen
        // by the player
        uint32_t score;

        // Used to force the player to walk through doors
        uint32_t current_room = 0;

        // For footstep sounds
        float elapsed_since_step = 0.f;
        bool first_step = false;

        float dog_treat_cooldown = 0.0f;
    };
}