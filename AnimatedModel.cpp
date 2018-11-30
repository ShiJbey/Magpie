#include "AnimatedModel.hpp"
#include "read_chunk.hpp"
#include "AssetLoader.hpp"

#include <iostream>
#include <fstream>
#include <assert.h>

///////////////////////////////////////////////////////
//                   MODEL DATA                      //
///////////////////////////////////////////////////////


Magpie::ModelData::ModelData(std::string const &filename) {
    static_assert(sizeof(HierarchyEntry) == 4 + 4 + 4 + 4*3 + 4*4 + 4*3, "HierarchyEntry is packed.");
    static_assert(sizeof(MeshEntry) == 4 + 4 + 4, "MeshEntry is packed.");

    std::ifstream file(filename, std::ios::binary);

	read_chunk(file, "str0", &names);

	read_chunk(file, "xfh0", &hierarchy);

	read_chunk(file, "msh0", &meshes);
};

///////////////////////////////////////////////////////
//                 ANIMATED MODEL                    //
///////////////////////////////////////////////////////

Magpie::AnimatedModel::AnimatedModel() {

};

Magpie::AnimatedModel::~AnimatedModel() {
    if (animation_manager != nullptr) {
        // Frees up the animation manager 
        // as well as the transform that our
        // 'Transform **transform' points to
        delete animation_manager;
    }
};

glm::vec3 Magpie::AnimatedModel::get_position() {
    if (transform != nullptr) {
        return (*transform)->position;
    }
    std::cout << "WARNING::AnimatedModel:: " << "Requesting position of transformless model." << std::endl;
    return glm::vec3(-9999.0f, -9999.0f, -9999.0f);
};

Magpie::AnimationManager* Magpie::AnimatedModel::get_animation_manager() {
    return animation_manager;
};

Scene::Transform** Magpie::AnimatedModel::get_transform() {
    return transform;
};

uint32_t Magpie::AnimatedModel::get_instance_id() {
	return this->instance_id;
};

void Magpie::AnimatedModel::set_position(glm::vec3 position) {
    if (transform != nullptr) {
        (*transform)->position = position;
        return;
    }
    std::cout << "WARNING::AnimatedModel:: " << "Setting position of transformless model." << std::endl;
};

void Magpie::AnimatedModel::set_transform(Scene::Transform** transform) {
    this->transform = transform;
    original_rotation = (*transform)->rotation;
};

void Magpie::AnimatedModel::set_animation_manager(Magpie::AnimationManager *animation_manager) {
    this->animation_manager = animation_manager;
};

void Magpie::AnimatedModel::set_model_orientation(uint32_t dir) {
    switch(dir) {
        case 3 :
            std::cout << "DEBUG::AnimatedModel Orienting right" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case 1 :
            std::cout << "DEBUG::AnimatedModel Orienting left" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case 2 :
            std::cout << "DEBUG::AnimatedModel:: Orienting up" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case 0:
            std::cout << "DEBUG::AnimatedModel:: Orienting down" << std::endl;
            (*transform)->rotation = original_rotation;
            break;
        default:
            std::cout << "ERROR::AnimatedModel:: Invalid orientation" << std::endl;
            break;
    }
};