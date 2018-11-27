#pragma once

#include "Clickable.hpp"
#include "AnimationManager.hpp"
#include "GameCharacter.hpp"

#include "Scene.hpp"
#include <glm/glm.hpp>

namespace Magpie {
    struct Door : public Clickable, public GameCharacter {

        // This is incremented each time we create a new player
        static uint32_t instance_count;

        Scene::Object* scene_object;
        
        // Positions on either side of the door
        glm::ivec2 room_a;
        glm::ivec2 room_b;

        bool locked;
        bool opened;

        Door(glm::ivec2 room_a, glm::ivec2 room_b, Scene::Object* obj);

        // Bouding box should have dimensions 1 x 1 x 2
        BoundingBox* get_boundingbox();

        // This should:
        // 1) Play the animation of the door opening
        void on_click();

        glm::vec3 get_position();
        Scene::Transform** get_transform();
        AnimationManager* get_animation_manager();
        uint32_t get_instance_id();

        // Loads Magpie model data specifically
        virtual Scene::Transform* load_model(Scene& scene, const ModelData* model_data, std::string model_name,
            std::function< void(Scene &, Scene::Transform *, std::string const &) > const &on_object);
        virtual std::vector< std::string > convert_animation_names(const TransformAnimation* tanim, std::string model_name);
    };
}