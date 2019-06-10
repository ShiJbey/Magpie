#pragma once

#include "../../base/Scene.hpp"

namespace Magpie{
    struct Wall {
            Scene::Object* scene_object;
            uint32_t room_number;

            Wall(Scene::Object* obj, uint32_t room_number) {
                this->scene_object = obj;
                this->room_number = room_number;
            }
    };
}
