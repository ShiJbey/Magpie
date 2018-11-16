#include "Player.hpp"

uint32_t Magpie::Player::instance_count = 0;

Magpie::Player::Player() {
    this->instance_id = instance_count;
    instance_count++;

    animation_manager = new AnimationManager();

    current_destination = glm::vec2(-0.0f, -0.0f);
    orientation = DIRECTION::LEFT;
    movespeed = 2.0f;
};

// Runs the lambda functions for each task on the hit list
// that has not been marked as completed
void Magpie::Player::update_hitlist() {
    for (uint32_t i = 0; i < hitlist.size(); i++) {
        //hitlist[i].completion_check(this);
    }
};

// Returns true if all the tasks in the hitlist are marked as
// completed
bool Magpie::Player::hitlist_complete() {
    for (uint32_t i = 0; i < hitlist.size(); i++) {
        if (!hitlist[i].completed) {
            return false;
        }
    }
    return true;
};

// Clean print out of the completion status of the player's tasks
void Magpie::Player::print_tasks() {
    for (uint32_t i = 0; i < hitlist.size(); i++) {
        std::cout << hitlist[i].to_string() << std::endl;
    }
};




void Magpie::Player::walk(float elapsed) {

    //printf("Current Destination: ( %f, %f)\n", current_destination.x, current_destination.y);
    //printf("Current Board Position: ( %f, %f)\n", board_position.x, board_position.y);
    //printf("Current Trans Position: ( %f, %f)\n", (*transform)->position.x, (*transform)->position.y);
    //turnTo(current_destination);
    if (current_destination == glm::vec2(-1.0f, -1.0f)) {
        current_destination = glm::vec2(get_position().x, get_position().y);
    }

    Player::set_position((*transform)->position + (velocity * elapsed));
    float distance_to_destination = glm::length(glm::vec3((float)current_destination.x, (float)current_destination.y, 0.0f) - get_position());
    
    if (distance_to_destination <= 0.01f || current_destination == glm::vec2(-1.0f, -1.0f || distance_to_destination > 2.0f)){//|| glm::dot(vector_to, getDirectionVec2()) < 0) {
        if (path.isEmpty()) {
            printf("DEBUG:: DONE PATH\n");
            Player::set_state((uint32_t)Player::STATE::IDLE);
            velocity = glm::vec3(0.0f, 0.0f, 0.0f);
            return;
        } else {
            
            Player::set_position(glm::vec3(current_destination, 0.0f));
            printf("DESTINATION REACHED\n");
            glm::vec2 next_destination = path.next();
            printf("NEXT DESTINATION: ( %f, %f)\n", next_destination.x, next_destination.y);
            turnTo(next_destination);
            current_destination = next_destination;
            glm::vec3 vec_to_next_destination = glm::vec3((float)current_destination.x, (float)current_destination.y, 0.0f) - get_position();
            if (glm::length(vec_to_next_destination) != 0.0f) {
                velocity = glm::normalize(vec_to_next_destination) * 2.0f;
            } else {
                velocity = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            set_model_orientation(orientation);
        }
    }
};

void Magpie::Player::consume_signal() {

};

void Magpie::Player::update(float elapsed) {
    animation_manager->update(elapsed);
    printf("Something is wrong.\n");
    /*
    if (current_state == (uint32_t)Player::STATE::WALKING) {
        walk(elapsed);
    }

    if (current_state == (uint32_t)Player::STATE::STEALING && animation_manager->get_current_animation()->animation_player->done()) {
        set_state((uint32_t)Player::STATE::IDLE);
        set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    */
};

void Magpie::Player::update_state(float elapsed) {

};

void Magpie::Player::interact() {

};

void Magpie::Player::set_position(glm::vec3 position) {
    Magpie::GameCharacter::set_position(position);
    board_position = glm::uvec2(position);
};

void Magpie::Player::set_state(uint32_t state) {
    GameAgent::set_state(state);
    animation_manager->set_current_state(state);
};

void Magpie::Player::set_score(uint32_t score) {
    this->score = score;
    printf("DEBUG:: Player score is (%d).", this->score);
};

uint32_t Magpie::Player::get_score() {
    return this->score;
};

/**
 * Loads a character model identically to how it imports scenes, 
 * however it modifies transform names to prevent, duplicate names
 */
Scene::Transform* Magpie::Player::load_model(Scene& scene, const ModelData* model_data, std::string model_name,
    std::function< void(Scene &, Scene::Transform *, std::string const &) > const &on_object) {

    std::vector< std::string > model_parts = { "GRP", "body", "head", "L_leg", "R_leg", "R_arm", "L_arm" };

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
                    std::string name = "Magpie_" + model_name + "_" + str + "_" + std::to_string(instance_id);
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

std::vector< std::string > Magpie::Player::convert_animation_names(const TransformAnimation* tanim, std::string model_name) {
    std::vector< std::string > modified_names;
    std::vector< std::string > model_parts = { "GRP", "body", "head", "L_leg", "R_leg", "R_arm", "L_arm" };
    for (const auto& name : tanim->names) {
        for (const auto& part : model_parts) {
            if (name.find(part) != std::string::npos && name.find("magpie") != std::string::npos) {
                modified_names.push_back("Magpie_" + model_name + "_" + part + "_" + std::to_string(instance_id));
                break;
            }
        }
    }
    return modified_names;
};

