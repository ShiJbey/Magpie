#pragma once

#include "GameAgent.hpp"
#include "Signalable.hpp"
#include "AnimatedModel.hpp"
#include "Player.hpp"

namespace Magpie {
    class Guard: public AnimatedModel, public GameAgent, public Signalable {
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
            DONUT
        };

        Guard();

        void walk(float elapsed);

        void consume_signal();

        void update(float elapsed);

        void update_state(float elapsed);

        void interact();

        void set_state(uint32_t state);

        void handle_state_idle(enum SIGHT);
        void handle_state_patrolling(enum SIGHT);
        void handle_state_cautious(enum SIGHT);
        void handle_state_alert(enum SIGHT);
        void handle_state_chasing(enum SIGHT);
        void handle_state_confused(enum SIGHT);
        uint32_t check_view();

        void set_patrol_points(std::vector<glm::vec3>);

        void set_position(glm::vec3 position);

        void setDestination(glm::vec3 destination);

        void turnTo(glm::vec3 destination);

        // Loads Magpie model data specifically
        Scene::Transform* load_model(Scene& scene, const ModelData* model_data, std::string model_name,
            std::function< void(Scene &, Scene::Transform *, std::string const &) > const &on_object);   

        virtual std::vector< std::string > convert_animation_names(const TransformAnimation* tanim, std::string model_name);

    private:
        float state_duration = 0.0f;
        enum STATE last_state = STATE::IDLE;

        std::vector<glm::vec3> patrol_points;
        glm::vec3 interest_point;
        int patrol_index = 0;
        bool cautious = false;
    };
}