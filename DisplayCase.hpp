#pragma once

#include "Clickable.hpp"
#include "AnimatedModel.hpp"
#include "Scene.hpp"

namespace Magpie {

    struct DisplayCase : public Clickable, public AnimatedModel {

        // Amount of time required to unlock the display case
        static float require_lock_pick_time;
        // How many display cases are there in the scene
        static uint32_t instance_count;
        // NOTE::This is only used to manage the pointer to the
        //        group transform
        Scene::Object *scene_object;

        DisplayCase();

        // Clickable functions
        BoundingBox* get_boundingbox();
        void on_click();

        // AnimatedModel functions
        virtual Scene::Transform* load_model(Scene& scene, const ModelData* model_data, std::string model_name,
            std::function< void(Scene &, Scene::Transform *, std::string const &) > const &on_object);
            
    };

}