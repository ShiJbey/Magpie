#pragma once

#include "../animation/AnimatedModel.hpp"
#include "../level/objects/Item.hpp"
#include "../level/MagpieLevel.hpp"

namespace Magpie {
    class Guard: public AnimatedModel, public GameAgent {
    public:

        Player* player;

        // This is incremented each time we create a new player
        static uint32_t instance_count;

        enum class STATE {
            IDLE = 0,
            PATROLING,
            CHASING,
            ALERT,
            CONFUSED,
            CAUTIOUS,
            EATING
        };

        enum class SIGHT {
            NOTHING,
            MAGPIE_ALERT,
            MAGPIE_NOTICE,
            TREAT
        };

        Guard();

        virtual void walk(float elapsed) override;
        void update(float elapsed) {}
        void update(float elapsed, MagpieLevel* level);
        // Updates atrtibutes related to the state of the Guard
        virtual void update_state(float elapsed, MagpieLevel* level);
        // Updates the state of the guard and the guards animation
        void set_state(uint32_t state);

        // State handlers
        void handle_state_idle(SIGHT view_state);
        void handle_state_patrolling(SIGHT view_state, float elapsed);
        void handle_state_cautious(SIGHT view_state, float elapsed);
        void handle_state_alert(SIGHT view_state);
        void handle_state_chasing(SIGHT view_state, float elapsed);
        void handle_state_confused(SIGHT view_state);
        void handle_state_eating(SIGHT view_state, MagpieLevel* level);

        // Checks to see if the player or any distracting
        // items are within the view of the, guard
        uint32_t check_view(MagpieLevel* level);

        void set_patrol_points(std::vector<glm::vec2>);

        void set_position(glm::vec3 position);

        void set_starting_point(glm::vec3 position);

        void turn_to(glm::vec2 loc);

        virtual void set_model_rotation(uint32_t dir) override;

        // Loads Guard character model data
        Scene::Transform* load_model(Scene& scene, const ModelData* model_data, std::string model_name,
            std::function< Scene::Object*(Scene &, Scene::Transform *, std::string const &) > const &on_object) override;

        // Modifies the names of the guard character model to support instancing
        virtual std::vector< std::string > convert_animation_names(const TransformAnimation* tanim, std::string model_name) override;



    private:
        // How long has the Guard been in the current state
        float state_duration = 0.0f;
        // What state was the guard in prior to the current one
        STATE previous_state = STATE::IDLE;
        // List of points that the guard patrols between
        std::vector< glm::vec2 > patrol_points;
        // Position of something of interest to the guard
        // (e.g. the player and dog treats)
        glm::vec2 interest_point;
        // What patrol point is the Guard navigating toward
        int patrol_point_index = 0;
    };
}