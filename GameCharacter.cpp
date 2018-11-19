#include "GameCharacter.hpp"
#include "read_chunk.hpp"

#include <iostream>
#include <fstream>
#include <assert.h>

Magpie::ModelData::ModelData(std::string const &filename) {
    static_assert(sizeof(HierarchyEntry) == 4 + 4 + 4 + 4*3 + 4*4 + 4*3, "HierarchyEntry is packed.");
    static_assert(sizeof(MeshEntry) == 4 + 4 + 4, "MeshEntry is packed.");

    std::ifstream file(filename, std::ios::binary);

	read_chunk(file, "str0", &names);

	read_chunk(file, "xfh0", &hierarchy);

	read_chunk(file, "msh0", &meshes);
};

Magpie::GameCharacter::GameCharacter() {

};

Magpie::GameCharacter::~GameCharacter() {

};

glm::vec3 Magpie::GameCharacter::get_position() {
    if (transform != nullptr) {
        return (*transform)->position;
    }
    return glm::vec3(-9999.0f, -9999.0f, -9999.0f);
};

Magpie::AnimationManager* Magpie::GameCharacter::get_animation_manager() {
    return animation_manager;
};

Scene::Transform** Magpie::GameCharacter::get_transform() {
    return transform;
};

uint32_t Magpie::GameCharacter::get_instance_id() {
	return this->instance_id;
};

void Magpie::GameCharacter::set_position(glm::vec3 position) {
    if (transform != nullptr) {
        (*transform)->position = position;
    }
};

void Magpie::GameCharacter::set_velocity(glm::vec3 velocity) {
    this->velocity = velocity;
};

void Magpie::GameCharacter::set_transform(Scene::Transform** transform) {
    this->transform = transform;
    original_rotation = (*transform)->rotation;
};

void Magpie::GameCharacter::set_model_orientation(GameAgent::DIRECTION dir) {
    switch(dir) {
        case GameAgent::DIRECTION::RIGHT :
            std::cout << "DEBUG:: Orienting right" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case GameAgent::DIRECTION::LEFT :
            std::cout << "DEBUG:: Orienting left" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case GameAgent::DIRECTION::UP :
            std::cout << "DEBUG:: Orienting up" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case GameAgent::DIRECTION::DOWN:
            std::cout << "DEBUG:: Orienting down" << std::endl;
            (*transform)->rotation = original_rotation;
            break;
        //case GameAgent::DIRECTION::UP_RIGHT :
        //    //std::cout << "DEBUG:: Facing up-right" << std::endl;
        //    (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //    break;
        //case GameAgent::DIRECTION::UP_LEFT :
        //    //std::cout << "DEBUG:: Facing up-left" << std::endl;
        //    (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //    break;
        //case GameAgent::DIRECTION::DOWN_RIGHT :
        //    //std::cout << "DEBUG:: right down-right" << std::endl;
        //    (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //    break;
        //case GameAgent::DIRECTION::DOWN_LEFT:
        //    //std::cout << "DEBUG:: right down-left" << std::endl;
        //    (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(315.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //    break;
        default:
            std::cout << "ERROR:: Invalid orientation" << std::endl;
            break;
    }
};