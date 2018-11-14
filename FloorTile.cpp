#include "FloorTile.hpp"

Magpie::FloorTile::FloorTile(Scene::Object* scene_object) {
    this->scene_object = scene_object;
};

Magpie::BoundingBox* Magpie::FloorTile::get_boundingbox() {
    // Return the existing bounding box
    if (this->bounding_box != nullptr) return this->bounding_box;
    // Create a new bounding box
    this->bounding_box = new BoundingBox(scene_object->transform->position, 
        glm::vec3(0.5f, 0.5f, 0.25f), 
        glm::vec3(-0.5f, -0.5f, -0.25f));
    return this->bounding_box;
};