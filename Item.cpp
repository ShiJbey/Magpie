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

uint32_t Magpie::Item::instance_count = 0;

Magpie::Item::Item(Scene::Object* obj) {
    this->scene_object = obj;
    this->instance_id = instance_count;
    instance_count++;
}

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

Magpie::Gem::Gem(Scene::Object* obj) : Item(obj) {
    this->selling_price = Gem::SELLING_PRICE;
    set_transform(&obj->transform);
    AnimatedModel::instance_id = Item::instance_id;
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

void Magpie::Gem::update_animation(float elapsed) {
    osc_tick += elapsed;
    if (osc_tick >= (2.0f * 3.14f))
        osc_tick = 0.0f;
    //std::cout << "pizza" << std::endl;
    //(*transform)->rotation  *= glm::angleAxis(glm::radians(10.0f), glm::vec3(1.0, 0.0, 0.0));
    (*transform)->position.z = 0.1f + std::sin(osc_tick * 3) * 0.1f;
};



//////////////////////////////////////////////
//                PAINTING                  //
//////////////////////////////////////////////

Magpie::Painting::Painting(): Magpie::Painting(nullptr) {
    // Do nothing
 };

Magpie::Painting::Painting (Scene::Object* obj_ptr) : Item(obj_ptr) {
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

//////////////////////////////////////////////
//                   GEODE                  //
//////////////////////////////////////////////

Magpie::Geode::Geode() : Geode(nullptr) {

};

Magpie::Geode::Geode(Scene::Object* obj_ptr) : Item(obj_ptr) {
    this->selling_price = SELLING_PRICE;
};