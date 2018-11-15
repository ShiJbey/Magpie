#include "Item.hpp"


//////////////////////////////////////////////
//                STEALABLE                 //
//////////////////////////////////////////////

uint32_t Magpie::Stealable::get_selling_price() {
    return this->selling_price;
};

void Magpie::Stealable::set_selling_price(uint32_t price) {
    this->selling_price = price;
};

void Magpie::Stealable::steal(Player* player) {
    player->set_score(player->get_score() + selling_price);
};

//////////////////////////////////////////////
//                  ITEM                    //
//////////////////////////////////////////////

void Magpie::Item::set_scene_object(Scene::Object* obj) {
    this->scene_object = obj;
};

Scene::Object* Magpie::Item::get_scene_object() {
    return this->scene_object;
};

//////////////////////////////////////////////
//                   GEM                    //
//////////////////////////////////////////////

Magpie::Gem::Gem() : Magpie::Gem(nullptr) {
    // Do Nothing
};

Magpie::Gem::Gem(Scene::Object* obj) {
    this->selling_price = Gem::SELLING_PRICE;
    this->scene_object = obj;
};

Magpie::BoundingBox* Magpie::Gem::get_boundingbox() {
    // Return the existing bounding box
    if (this->bounding_box != nullptr) return this->bounding_box;
    // Create a new bounding box
    this->bounding_box = new BoundingBox(scene_object->transform->position, glm::vec3(0.5f, 0.5f, 2.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
    return this->bounding_box;
};

void Magpie::Gem::on_click() {
    if (scene_object->active) {
        printf("DEBUG:: Gem has been clicked.\n");
        scene_object->active = false;
    }
};


//////////////////////////////////////////////
//                PAINTING                  //
//////////////////////////////////////////////

Magpie::Painting::Painting(): Magpie::Painting(nullptr) {
    // Do nothing
 };

Magpie::Painting::Painting (Scene::Object* obj_ptr) {
    this->scene_object = obj_ptr;
    this->selling_price = Painting::SELLING_PRICE;
};

void Magpie::Painting::on_click() {
    if (scene_object->active) {
        printf("DEBUG:: Painting has been clicked.\n");
        scene_object->active = false;
    }
};

Magpie::BoundingBox* Magpie::Painting::get_boundingbox() {
    // Return the existing bounding box
    if (this->bounding_box != nullptr) return this->bounding_box;
    // Create a new bounding box
    this->bounding_box = new BoundingBox(this->scene_object->transform->position, glm::vec3(0.5f, 0.5f, 2.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
    return bounding_box;
};