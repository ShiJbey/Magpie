#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Bounding boxes are used during the click ray casting
// to see if an object intersects with the ray
struct BoundingBox {
    glm::vec3 bottom_front_left_corner;
    float length;   // Dimension in the positive x-direction
    float width;    // Dimension in the positive y-direction
    float height;   // Dimension in the positive z-direction  
};

// This interface is inherited by all objects that are
// clickable
class Clickable {
public:
    bool active = true;
    virtual ~Clickable() { }
    virtual void click() = 0;
    virtual BoundingBox get_boundingbox() = 0;
    void set_active(bool _active) { this->active = _active; }
};