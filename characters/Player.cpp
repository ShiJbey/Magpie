#include "Player.hpp"
#include "../AssetLoader.hpp"
#include "../game_settings.hpp"

uint32_t Magpie::Player::instance_count = 0;

Magpie::Player::Player() {
    this->instance_id = instance_count;
    instance_count++;

    animation_manager = new AnimationManager();

    score = 0;
    movespeed = 2.0f;
};

bool Magpie::Player::is_disguised() {
    return current_state == (uint32_t)STATE::DISGUISE_IDLE;
};

void Magpie::Player::walk(float elapsed) {
        // Check to see if we are at our current_destination
        // If we are, then update the current destination
        // Move towards the current destination at the movement speed

        glm::vec2 current_position_v2  = get_position();
        current_destination = path[path_destination_index];
        float distance_to_destination = glm::length(current_destination - current_position_v2);

        // Stop them from walking off some random place
        if (distance_to_destination > 1) {
            set_position(glm::round(get_position()));
            this->current_destination = get_position();
            if (current_state == (uint32_t)Player::STATE::WALKING) {
                Player::set_state((uint32_t)Player::STATE::IDLE);
            }
            else if (current_state == (uint32_t)Player::STATE::DISGUISE_WALK) {
                Player::set_state((uint32_t)Player::STATE::DISGUISE_IDLE);
            }

            // Clear the path and resent destination index
            this->path.clear();
            this->path_destination_index = 0;
            return;
        }

        // Consider ourselves to be at the position, snap to it, and return
        if (distance_to_destination < 0.001) {
            set_position(glm::vec3((int)current_destination.x, (int)current_destination.y, 0.0f));
            path_destination_index++;

            // Stop Walking because we are at the end of our path
            if (path_destination_index >= path.size()) {
                if (current_state == (uint32_t)Player::STATE::WALKING) {
                    Player::set_state((uint32_t)Player::STATE::IDLE);
                }
                else if (current_state == (uint32_t)Player::STATE::DISGUISE_WALK) {
                    Player::set_state((uint32_t)Player::STATE::DISGUISE_IDLE);
                }

                // Clear the path and resent destination index
                this->path.clear();
                this->path_destination_index = 0;
                return;
            }
            // Set the destination to the next position in the path and face towards it
            else {
                previous_destination = current_destination;
                current_destination = path[path_destination_index];
                set_destination(current_destination);
                turn_to(current_destination);
            }
        }
        // Move towards the current destination
        else {
            glm::vec2 potential_elapsed_movement = elapsed * movespeed * get_facing_direction_as_vec2();
            glm::vec2 elapsed_movement = (glm::length(potential_elapsed_movement) <= distance_to_destination) ?
                potential_elapsed_movement : current_destination - current_position_v2;
            set_position(get_position() + glm::vec3(elapsed_movement.x, elapsed_movement.y, 0.0f));
        }
};

void Magpie::Player::turn_to(glm::vec2 loc) {
    glm::vec3 current_position = get_position();

    DIRECTION direction_to_loc = GameAgent::direction_toward(
        glm::vec2(current_position.x, current_position.y),
        loc
    );

    set_facing_direction(direction_to_loc);
    set_model_rotation((uint32_t)direction_to_loc);
};

void Magpie::Player::update(float elapsed) {
    animation_manager->update(elapsed);

    // Decrement the cool down time for dropping treats
    dog_treat_cooldown -= elapsed;

    if (current_state == (uint32_t)Player::STATE::WALKING || current_state == (uint32_t)Player::STATE::DISGUISE_WALK) {
        walk(elapsed);

        //play footsteps if walking
        elapsed_since_step += elapsed;
        if ((get_state() == (uint32_t)Player::STATE::WALKING && elapsed_since_step >= 0.33f) ||
            elapsed_since_step >= .67f) {
            elapsed_since_step = 0.f;
            first_step = !first_step;

            if (!mute_sound) {
                if (first_step) {
                    sample_magpie_walk1->play(get_position(), 0.2f);
                } else {
                    sample_magpie_walk2->play(get_position(), 0.3f);
                }
            }

        }
    }

    if (current_state == (uint32_t)Player::STATE::STEALING && animation_manager->get_current_animation()->animation_player->done()) {
        set_state((uint32_t)Player::STATE::IDLE);
    }

    if (current_state == (uint32_t)Player::STATE::PICKING && animation_manager->get_current_animation()->animation_player->done()) {
        set_state((uint32_t)Player::STATE::IDLE);
    }
};

void Magpie::Player::set_position(glm::vec3 position) {
    Magpie::AnimatedModel::set_position(position);
    Sound::listener.set_position(get_position());
};

void Magpie::Player::set_state(uint32_t state) {
    GameAgent::set_state(state);
    animation_manager->set_current_state(state);

    if (state == (uint32_t)Player::STATE::WALKING || state == (uint32_t)Player::STATE::DISGUISE_WALK) {
        elapsed_since_step = 1.f;
        first_step = false;
    }
};

void Magpie::Player::set_score(uint32_t score) {
    this->score = score;
    //printf("DEBUG:: Player score is (%d).", this->score);
};

uint32_t Magpie::Player::get_score() {
    return this->score;
};

/**
 * Loads a character model identically to how it imports scenes,
 * however it modifies transform names to prevent, duplicate names
 */
Scene::Transform* Magpie::Player::load_model(Scene& scene, const ModelData* model_data, std::string model_name,
    std::function< Scene::Object*(Scene &, Scene::Transform *, std::string const &) > const &on_object) {

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

/**
 * Names of the transforms associated with animations need to be changes to fit the instancing
 * naming convention
 */
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

void Magpie::Player::set_current_room(uint32_t room_number) {
    this->current_room = room_number;
};

uint32_t Magpie::Player::get_current_room() {
    return this->current_room;
};

void Magpie::Player::set_model_rotation(uint32_t dir) {
    switch(dir) {
        case (uint32_t)GameAgent::DIRECTION::RIGHT :
            //std::cout << "DEBUG::Player.set_model_rotation:: Orienting right" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case (uint32_t)GameAgent::DIRECTION::LEFT :
            //std::cout << "DEBUG::Player.set_model_rotation:: Orienting left" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case (uint32_t)GameAgent::DIRECTION::UP :
            //std::cout << "DEBUG::Player.set_model_rotation:: Orienting up" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case (uint32_t)GameAgent::DIRECTION::DOWN:
            //std::cout << "DEBUG::Player.set_model_rotation:: Orienting down" << std::endl;
            (*transform)->rotation = original_rotation;
            break;
        default:
            std::cout << "ERROR:: Invalid orientation" << std::endl;
            break;
    }
};

void Magpie::Player::reset_treat_cooldown() {
    set_treat_cooldown(2.0f);
};

void Magpie::Player::set_treat_cooldown(float cooldown_time) {
    this->dog_treat_cooldown = cooldown_time;
};

bool Magpie::Player::can_place_treat() {
    return this->dog_treat_cooldown <= 0.0f;
};