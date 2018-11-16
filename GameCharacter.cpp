#include "GameCharacter.hpp"
#include "read_chunk.hpp"

#include <iostream>
#include <fstream>
#include <assert.h>

Magpie::ModelData::ModelData(std::string const &filename) {
    static_assert(sizeof(HierarchyEntry) == 4 + 4 + 4 + 4*3 + 4*4 + 4*3, "HierarchyEntry is packed.");
    static_assert(sizeof(MeshEntry) == 4 + 4 + 4, "MeshEntry is packed.");

    std::ifstream file(filename, std::ios::binary);

	std::vector< char > names;

	read_chunk(file, "str0", &names);

	read_chunk(file, "xfh0", &hierarchy);

	read_chunk(file, "msh0", &meshes);
};

Magpie::GameCharacter::GameCharacter() {

};

Magpie::GameCharacter::~GameCharacter() {

};

/**
 * Loads a character model identically to how it imports scenes, 
 * however it modifies transform names to prevent, duplicate names
 */
void Magpie::GameCharacter::load_character_model(Scene& scene, const ModelData* model_data, 
    std::function< void(Scene &, Scene::Transform *, std::string const &) > const &on_object) {

    std::vector< Scene::Transform * > hierarchy_transforms;
	hierarchy_transforms.reserve(model_data->hierarchy.size());

    for (auto const &h : model_data->hierarchy) {
		Scene::Transform *t = scene.new_transform();
		if (h.parent != -1U) {
			if (h.parent >= hierarchy_transforms.size()) {
				throw std::runtime_error("Model data did not contain transforms in topological-sort order.");
			}
			t->set_parent(hierarchy_transforms[h.parent]);
		}

		if (h.name_begin <= h.name_end && h.name_end <= model_data->names.size()) {
			t->name = std::string(model_data->names.begin() + h.name_begin, model_data->names.begin() + h.name_end);
		} else {
				throw std::runtime_error("Model data contains hierarchy entry with invalid name indices");
		}

		t->position = h.position;
		t->rotation = h.rotation;
		t->scale = h.scale;

		hierarchy_transforms.emplace_back(t);
	}

	assert(hierarchy_transforms.size() == model_data->hierarchy.size());

    for (auto const &m : model_data->meshes) {
		if (m.transform >= hierarchy_transforms.size()) {
			throw std::runtime_error("Model data contains mesh entry with invalid transform index (" + std::to_string(m.transform) + ")");
		}
		if (!(m.name_begin <= m.name_end && m.name_end <= model_data->names.size())) {
			throw std::runtime_error("Model data contains mesh entry with invalid name indices");
		}
		std::string name = std::string(model_data->names.begin() + m.name_begin, model_data->names.begin() + m.name_end);

		if (on_object) {
			on_object(scene, hierarchy_transforms[m.transform], name);
		}

	}
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
            //std::cout << "DEBUG:: Facing right" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case GameAgent::DIRECTION::LEFT :
            //std::cout << "DEBUG:: Facing left" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case GameAgent::DIRECTION::UP :
            //std::cout << "DEBUG:: Facing right" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case GameAgent::DIRECTION::DOWN:
            //std::cout << "DEBUG:: Facing down" << std::endl;
            (*transform)->rotation = original_rotation;
            break;
        case GameAgent::DIRECTION::UP_RIGHT :
            //std::cout << "DEBUG:: Facing up-right" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case GameAgent::DIRECTION::UP_LEFT :
            //std::cout << "DEBUG:: Facing up-left" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case GameAgent::DIRECTION::DOWN_RIGHT :
            //std::cout << "DEBUG:: right down-right" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case GameAgent::DIRECTION::DOWN_LEFT:
            //std::cout << "DEBUG:: right down-left" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(315.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
    }
};