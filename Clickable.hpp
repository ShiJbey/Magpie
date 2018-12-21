#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Magpie {
    // Bounding boxes are used during the click ray casting
    // to see if an object intersects with the ray
    struct BoundingBox {
        glm::vec3 position; // Should be the same as the object it attached to
        glm::vec3 bb_max;   // Positive offsets from the position 
        glm::vec3 bb_min;   // Negative offsets from the position

        BoundingBox(glm::vec3 position, glm::vec3 bb_max, glm::vec3 bb_min) {
            this->position = position;
            this->bb_max = bb_max;
            this->bb_min = bb_min;
        }

        // TODO::Move to separate file
        // From: https://stackoverflow.com/questions/20140711/picking-in-3d-with-ray-tracing-using-ninevehgl-or-opengl-i-phone/20143963#20143963
        // http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
        bool check_intersect(const glm::vec3 &ray_origin, const glm::vec3 &ray_dir);
    };

    // This interface is inherited by all objects that are
    // clickable
    class Clickable {
    public:
        bool active = true;
        BoundingBox* bounding_box = nullptr;
        virtual ~Clickable() { }
        virtual void on_click() = 0;
        virtual BoundingBox* get_boundingbox() = 0;
        void set_active(bool _active) { this->active = _active; }
    };
}
