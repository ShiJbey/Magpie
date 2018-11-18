#include "Guard.hpp"

#include <iostream>

uint32_t Magpie::Guard::instance_count = 0;

Magpie::Guard::Guard() {
    this->instance_id = instance_count;
    instance_count++;

    animation_manager = new AnimationManager();
};

void Magpie::Guard::walk(float elapsed) {
    /*
    float distance = elapsed * movespeed;

    glm::vec3 vector_to =  current_destination - glm::vec3(board_position);

//    std::cout << "Walking to " << cur_destination.x << "," << cur_destination.y << "from" << position.x << ","
//              << position.y << "Direction" << orientation << ":" << getDirectionVec2().x << "," << getDirectionVec2().y << std::endl;

    if (glm::length(vector_to) < distance || glm::dot(glm::vec2(vector_to.x, vector_to.y), getDirectionVec2()) < 0) {

        board_position = current_destination;
        if (path.isEmpty()) {
            std::cout << "EMPTY" << std::endl;
            if (current_state != (uint32_t)Guard::STATE::CHASING) current_state = (uint32_t)Guard::STATE::IDLE;
        } else {
            std::cout << "NEXT" << std::endl;
            glm::vec2 next_destination = path.next();
            current_destination = glm::vec3((float)next_destination.x, next_destination.y, 0.0f);
            turnTo(current_destination);
        }
    } else {
        board_position += getDirectionVec2() * distance;
    }
    */
};

void Magpie::Guard::consume_signal() {

};

void Magpie::Guard::update(float elapsed) {
    animation_manager->update(elapsed);
};

void Magpie::Guard::update_state(float elapsed) {

};

void Magpie::Guard::set_state(uint32_t state) {
    GameAgent::set_state(state);
    animation_manager->set_current_state(state);
};

void Magpie::Guard::interact() {

};

/**
 * Loads a character model identically to how it imports scenes, 
 * however it modifies transform names to prevent, duplicate names
 */
Scene::Transform* Magpie::Guard::load_model(Scene& scene, const ModelData* model_data, std::string model_name,
    std::function< void(Scene &, Scene::Transform *, std::string const &) > const &on_object) {

    std::vector< std::string > model_parts = { "GRP", "body", "head", "L_leg", "R_leg", "R_arm", "L_arm", "hat", "exclaim", "tail", "question" };

    Scene::Transform* model_group_transform = nullptr;

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
            // NOTE:: Here we need to do some checks on the transform names to ensure
            //      Transforms are named differently

            // Get the name from the char vector
			std::string exported_name = std::string(model_data->names.begin() + h.name_begin, model_data->names.begin() + h.name_end);

            // Check if the name contains any of the model part string
            for (auto str: model_parts) {
                if (exported_name.find(str) != std::string::npos) {
                    // We have a name match, now set a new name with the instance ID
                    // (e.g. Magpie_body_0)
                    std::string name = "Guard_" + model_name + "_" + str + "_" + std::to_string(instance_id);
                    t->name = name;

                    if (str.compare("GRP") == 0) {
                        model_group_transform = t;
                    }
                }
            }

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
    
    return model_group_transform;
};

std::vector< std::string > Magpie::Guard::convert_animation_names(const TransformAnimation* tanim, std::string model_name) {
    std::vector< std::string > modified_names;
    std::vector< std::string > model_parts = { "GRP", "body", "head", "L_leg", "R_leg", "R_arm", "L_arm", "hat", "exclaim", "tail", "question" };
    for (const auto& name : tanim->names) {
        for (const auto& part : model_parts) {
            if (name.find(part) != std::string::npos && name.find("guard") != std::string::npos) {
                modified_names.push_back("Guard_" + model_name + "_" + part + "_" + std::to_string(instance_id));
            }
        }
    }
    return modified_names;
};