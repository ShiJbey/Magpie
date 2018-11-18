#pragma once

#include "Scene.hpp"

namespace Magpie {
    struct Door {

        Scene::Object* scene_object;

        uint32_t room_a;
        uint32_t room_b;

        bool locked;

    };
}