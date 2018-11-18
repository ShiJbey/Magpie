#pragma once

#include "Clickable.hpp"
#include "Scene.hpp"

namespace Magpie {

    struct FloorTile {
        FloorTile(): FloorTile(nullptr, 0) {};
        FloorTile(Scene::Object *scene_object, uint32_t room_number);
        //BoundingBox* get_boundingbox();
        Scene::Object *scene_object;
        uint32_t room_number;
    };

}