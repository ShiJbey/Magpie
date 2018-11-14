#pragma once

#include "Clickable.hpp"
#include "Scene.hpp"

namespace Magpie {

    class FloorTile : public Clickable{
    public:
        FloorTile( Scene::Object *scene_object);
        BoundingBox* get_boundingbox();
    protected:
        Scene::Object *scene_object;
    };

}