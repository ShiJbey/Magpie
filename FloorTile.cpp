#include "FloorTile.hpp"

Magpie::FloorTile::FloorTile(Scene::Object* scene_object, uint32_t room_number) {
    this->scene_object = scene_object;
    this->room_number = room_number;
};