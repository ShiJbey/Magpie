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
        scene_object->active = false;
    }
};

void Magpie::Gem::update_animation(float elapsed) {
    osc_tick += elapsed;
    
    if (osc_tick >= (2.0f * 3.14f))
        osc_tick = 0.0f;

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
    set_transform(&obj_ptr->transform);
    AnimatedModel::instance_id = Item::instance_id;
};

void Magpie::Painting::on_click() {
    if (scene_object->active) {
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

void Magpie::Painting::update_animation(float elapsed) {
    osc_tick += elapsed;

    if (osc_tick >= (2.0f * 3.14f))
        osc_tick = 0.0f;
        
    (*transform)->position.z = std::sin(osc_tick * 3) * 0.05f;
    (*transform)->position.z = std::cos(osc_tick * 5) * 0.05f;
};


//////////////////////////////////////////////
//                   GEODE                  //
//////////////////////////////////////////////

Magpie::Geode::Geode() : Geode(nullptr) {

};

Magpie::Geode::Geode(Scene::Object* obj_ptr) : Item(obj_ptr) {
    this->selling_price = SELLING_PRICE;
    set_transform(&obj_ptr->transform);
    AnimatedModel::instance_id = Item::instance_id;
};

void Magpie::Geode::update_animation(float elapsed) {
    osc_tick += elapsed;

    if (osc_tick >= (2.0f * 3.14f))
        osc_tick = 0.0f;
        
    (*transform)->rotation *= glm::angleAxis(glm::radians(std::sin(osc_tick * 3.0f) * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
};

//////////////////////////////////////////////
//               KEY CARD                   //
//////////////////////////////////////////////

Magpie::KeyCard::KeyCard(Door::ACCESS_LEVEL access_level): Magpie::KeyCard(access_level, nullptr) {
    
};

Magpie::KeyCard::KeyCard(Door::ACCESS_LEVEL access_level, Scene::Object* obj_ptr) : Item(obj_ptr) {
    set_transform(&obj_ptr->transform);
    AnimatedModel::instance_id = Item::instance_id;

};

void Magpie::KeyCard::on_click() {
    if (scene_object->active) {
        scene_object->active = false;
    }
};

Magpie::BoundingBox* Magpie::KeyCard::get_boundingbox() {
    // Return the existing bounding box
    if (this->bounding_box != nullptr) return this->bounding_box;
    // Create a new bounding box
    this->bounding_box = new BoundingBox(this->scene_object->transform->position, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, -0.5f));
    return bounding_box;
};

void Magpie::KeyCard::update_animation(float elapsed) {
    osc_tick += elapsed;

    if (osc_tick >= (2.0f * 3.14f))
        osc_tick = 0.0f;

    (*transform)->position.z = std::cos(osc_tick * 6) * 0.05f;
    (*transform)->position.z += 0.05f;
};

//////////////////////////////////////////////
//               DOG TREAT                  //
//////////////////////////////////////////////

Magpie::DogTreat::DogTreat(): Magpie::DogTreat(nullptr) {

};

Magpie::DogTreat::DogTreat(Scene::Object* obj_ptr) : Item(obj_ptr) {
    set_transform(&obj_ptr->transform);
    AnimatedModel::instance_id = Item::instance_id;
};

void Magpie::DogTreat::on_click() {
    if (scene_object->active) {
        scene_object->active = false;
    }
};

Magpie::BoundingBox* Magpie::DogTreat::get_boundingbox() {
    // Return the existing bounding box
    if (this->bounding_box != nullptr) return this->bounding_box;
    // Create a new bounding box
    this->bounding_box = new BoundingBox(this->scene_object->transform->position, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, -0.5f));
    return bounding_box;
};

void Magpie::DogTreat::update_animation(float elapsed) {
    osc_tick += elapsed;

    if (osc_tick >= (2.0f * 3.14f))
        osc_tick = 0.0f;

    (*transform)->position.z = std::cos(osc_tick * 6) * 0.05f;
    (*transform)->position.z += 0.05f;
};

//////////////////////////////////////////////
//            CARDBOARD BOX                 //
//////////////////////////////////////////////

Magpie::CardboardBox::CardboardBox(): Magpie::CardboardBox(nullptr) {

};

Magpie::CardboardBox::CardboardBox(Scene::Object* obj_ptr) : Item(obj_ptr) {
    set_transform(&obj_ptr->transform);
    AnimatedModel::instance_id = Item::instance_id;
};

void Magpie::CardboardBox::on_click() {
    if (scene_object->active) {
        scene_object->active = false;
    }
};

Magpie::BoundingBox* Magpie::CardboardBox::get_boundingbox() {
    // Return the existing bounding box
    if (this->bounding_box != nullptr) return this->bounding_box;
    // Create a new bounding box
    this->bounding_box = new BoundingBox(this->scene_object->transform->position, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, -0.5f));
    return bounding_box;
};

void Magpie::CardboardBox::update_animation(float elapsed) {
    osc_tick += elapsed;

    if (osc_tick >= (2.0f * 3.14f))
        osc_tick = 0.0f;

    (*transform)->position.z = std::cos(osc_tick * 6) * 0.05f;
    (*transform)->position.z += 0.05f;
};

//////////////////////////////////////////////
//              BACK EXIT                   //
//////////////////////////////////////////////

Magpie::BackExit::BackExit(): Magpie::BackExit(nullptr) {

};

Magpie::BackExit::BackExit(Scene::Object* obj_ptr) : Item(obj_ptr) {
    set_transform(&obj_ptr->transform);
    AnimatedModel::instance_id = Item::instance_id;
};

void Magpie::BackExit::on_click() {
};

Magpie::BoundingBox* Magpie::BackExit::get_boundingbox() {
    // Return the existing bounding box
    if (this->bounding_box != nullptr) return this->bounding_box;
    // Create a new bounding box
    this->bounding_box = new BoundingBox(this->scene_object->transform->position, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, -0.5f));
    return bounding_box;
};