#pragma once

#include "Clickable.hpp"
#include "Scene.hpp"

namespace Magpie {

    struct FloorTile {
        FloorTile(): FloorTile(nullptr) {};
        FloorTile( Scene::Object *scene_object);
        //BoundingBox* get_boundingbox();
        Scene::Object *scene_object;
    };

}