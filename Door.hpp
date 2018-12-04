#pragma once

#include "Clickable.hpp"
#include "AnimationManager.hpp"
#include "AnimatedModel.hpp"

#include "Scene.hpp"
#include <glm/glm.hpp>
#include <map>

namespace Magpie {
    struct Door : public Clickable, public AnimatedModel {

        enum class ACCESS_LEVEL {
            NORMAL = 0,
            PINK,
            GREEN,
        };

        // This is incremented each time we create a new player
        static uint32_t instance_count;

        Scene::Object* scene_object;
        
        // Positions on either side of the door
        glm::ivec2 room_a;
        glm::ivec2 room_b;

        std::map<uint32_t, glm::ivec2> rooms;

        bool locked;
        bool opened;

        ACCESS_LEVEL access_level;

        Door();
        Door(glm::ivec2 room_a, glm::ivec2 room_b, Scene::Object* obj);

        // Clickable functions
        BoundingBox* get_boundingbox();
        void on_click();


        // Animated model functions
        virtual Scene::Transform* load_model(Scene& scene, const ModelData* model_data, std::string model_name,
            std::function< Scene::Object*(Scene &, Scene::Transform *, std::string const &) > const &on_object);
        virtual std::vector< std::string > convert_animation_names(const TransformAnimation* tanim, std::string model_name);
    };
}