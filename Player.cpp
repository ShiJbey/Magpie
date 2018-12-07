#include "Player.hpp"
#include "AssetLoader.hpp"

uint32_t Magpie::Player::instance_count = 0;

Magpie::Player::Player() {
    this->instance_id = instance_count;
    instance_count++;

    animation_manager = new AnimationManager();

    score = 0;

    current_destination = glm::ivec3(0, 0, 0);
    orientation = DIRECTION::LEFT;
    movespeed = 2.0f;
};

bool Magpie::Player::is_disguised() {
    return current_state == (uint32_t)STATE::DISGUISE_IDLE;
};

void Magpie::Player::setDestination(glm::vec3 destination) {
    current_destination = destination;
    Player::turnTo(current_destination);
    set_model_orientation((uint32_t)orientation);
};

void Magpie::Player::walk(float elapsed) {

    if (is_new_path) {
        
        is_new_path = false;
        path = new_path;

        next_destination_index = 0;

//        printf("=====Starting on a new path=====\n");
//        printf("\tBeginning Destination Index: %d\n", next_destination_index);
//        printf("\tStarting Position: (%f, %f, %f)\n", get_position().x, get_position().y, get_position().z);

		glm::vec3 next_destination = glm::vec3(path.get_path()[next_destination_index], 0.0f);
        next_destination_index++;
        current_destination = glm::vec3(next_destination.x, next_destination.y, 0.0f);

//        printf("\t#### Destination Set ####\n");
//        printf("\tCurrent Destination: (%f, %f, %f)\n", current_destination.x, current_destination.y, current_destination.z);
//        printf("\tNext Destination Index is now: %d\n", next_destination_index);

        accumulate_time = 0;
		turnTo(current_destination);
		set_model_orientation((uint32_t)orientation);
		return;
    }

	if (next_destination_index > this->path.get_path().size())
	{
        if (current_state == (uint32_t)Player::STATE::WALKING) {
            Player::set_state((uint32_t)Player::STATE::IDLE);
        }
        else if (current_state == (uint32_t)Player::STATE::DISGUISE_WALK) {
            Player::set_state((uint32_t)Player::STATE::DISGUISE_IDLE);
        }

		next_destination_index = 0;
		return;
	}

	if (accumulate_time == 0)
	{
		starting_point = get_position();
	}

	accumulate_time += elapsed;

    glm::vec3 vector_to = current_destination - glm::vec3(starting_point.x, starting_point.y, 0.0f);

    // (0.1, 0) -> (1.0, 0)
    if (vector_to.x != 0) vector_to.x = vector_to.x / abs(vector_to.x);
    if (vector_to.y != 0) vector_to.y = vector_to.y / abs(vector_to.y);



	glm::vec2 moving_distance = accumulate_time * vector_to * movespeed;

	glm::vec3 current_position = starting_point + glm::vec3(moving_distance, 0.0f);

	if (glm::length(moving_distance) >= glm::length(vector_to))
	{
		accumulate_time = 0;
		current_position = current_destination;
        if (next_destination_index < this->path.get_path().size()) {
            glm::vec2 next_destination = path.get_path()[next_destination_index++];
		    current_destination = glm::vec3(next_destination.x, next_destination.y, 0.0f); 
        } else {
            next_destination_index++;
        }

//        printf("\t#### Destination Set ####\n");
//        printf("\tCurent Postition: (%f, %f, %f)\n", get_position().x, get_position().y, get_position().z);
//        printf("\tCurrent Destination: (%f, %f, %f)\n", current_destination.x, current_destination.y, current_destination.z);
//        printf("\tNext Destination Index is now: %d\n", next_destination_index);
        turnTo(current_destination);
        set_model_orientation((uint32_t)orientation);
    
        
	}

    Player::set_position(current_position);
};

void Magpie::Player::consume_signal() {

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

            if (first_step) {
                sample_magpie_walk1->play(get_position(), 0.2f);
            } else {
                sample_magpie_walk2->play(get_position(), 0.1f);
            }
        }
    }

    if ((current_state == (uint32_t)Player::STATE::STEALING && animation_manager->get_current_animation()->animation_player->done())
        || (current_state == (uint32_t)Player::STATE::PICKING && animation_manager->get_current_animation()->animation_player->done())) {
        set_state((uint32_t)Player::STATE::IDLE);
    }
};

void Magpie::Player::update_state(float elapsed) {

};

void Magpie::Player::interact() {

};

void Magpie::Player::set_position(glm::vec3 position) {
    Magpie::AnimatedModel::set_position(position);
    board_position = glm::ivec3((int)position.x, (int)position.y, 0);
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

void Magpie::Player::turnTo(glm::vec3 destination) {  

    float x_difference = destination.x - get_position().x;
    float y_difference = destination.y - get_position().y;

    // WARNING::Only handles movement in cardinal directions
    if (abs(x_difference) > abs(y_difference)) {
        if (x_difference > 0) {
//            std::cout << "DEBUG::Player.turnTo():: Facing right" << std::endl;
            orientation = DIRECTION::RIGHT;
        }
        else if (x_difference < 0){
//            std::cout << "DEBUG::Player.turnTo():: Facing left" << std::endl;
            orientation = DIRECTION::LEFT;
        }
    } else {
        if (y_difference > 0) {
//            std::cout << "DEBUG::Player.turnTo():: Facing up" << std::endl;
            orientation = DIRECTION::UP;
        } else if (y_difference < 0){
//            std::cout << "DEBUG::Player.turnTo():: Facing down" << std::endl;
            orientation = DIRECTION::DOWN;
        }
    }
};

void Magpie::Player::set_model_orientation(uint32_t dir) {
    switch(dir) {
        case (uint32_t)GameAgent::DIRECTION::RIGHT :
            //std::cout << "DEBUG::Player.set_model_orientation:: Orienting right" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case (uint32_t)GameAgent::DIRECTION::LEFT :
            //std::cout << "DEBUG::Player.set_model_orientation:: Orienting left" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case (uint32_t)GameAgent::DIRECTION::UP :
            //std::cout << "DEBUG::Player.set_model_orientation:: Orienting up" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case (uint32_t)GameAgent::DIRECTION::DOWN:
            //std::cout << "DEBUG::Player.set_model_orientation:: Orienting down" << std::endl;
            (*transform)->rotation = original_rotation;
            break;
        default:
            std::cout << "ERROR:: Invalid orientation" << std::endl;
            break;
    }
};

void Magpie::Player::set_path(Magpie::Path path) {

    // Do nothing for empty path
    if (path.get_path().size() == 0) {
        //std::cout << "DEBUG::Player.set_path():: EMPTY PATH" << std::endl;
        return;
    }

    // The magpie has finished the previous path and this one
    // should replace the old one
    if (this->next_destination_index > this->path.get_path().size() + 1 ||
        this->next_destination_index == 0 || this->path.get_path().size() == 0) {
        
        this->is_new_path = true;
        this->new_path = path;
        this->next_destination_index = 0;
    }

    // The player has clicked for the magpie to move on a different path
    // while the Magpie was currently navigating a path
    else {
        //std::cout << "DEBUG::Player.set_path():: Appending new path to previous\n" << std::endl;

        //std::cout << "DEBUG::Player.set_path():: Current Path" << std::endl;
        //for (uint32_t i = 0; i < next_destination_index; i++) {
        //    glm::vec2 pos = this->path.get_path()[i];
        //    printf("(%f, %f)\n", pos.x, pos.y);
        //}

        //std::cout << "DEBUG::Player.set_path():: Given Path" << std::endl;
        //for (auto &pos: path.get_path()) {
        //    printf("(%f, %f)\n", pos.x, pos.y);
        //}

        // Remove all locations in the path vector after the current destination
        // Append this path to the end of the old path and let the magpie continue
        // as normal
        std::vector<glm::vec2> modified_path = this->path.get_path();
        std::vector<glm::vec2> new_path = path.get_path();

        // Ignore if the new path is taking you to the same place
        // as your current path
        if (new_path.back() == this->get_path()->get_path().back()) {
            return;
        }

        // Round the player's current position
//        glm::vec3 rounded_position = glm::round(get_position());

        // Erase all locations after the next destination
        modified_path.erase(modified_path.begin() + next_destination_index, modified_path.end());

        // Remove overlapping destinations
        if (modified_path.size() >= 2 && new_path.size() >= 2) {
//            auto mp_last_elm_iter = modified_path.rbegin() + 2;
//            auto mp_2nd_last_elm_iter = modified_path.rbegin() + 1;
//            auto np_1st_elm_iter = new_path.begin() + 1;
            auto np_2nd_elm_iter = new_path.begin() + 1;
            if (modified_path[modified_path.size() - 2] == new_path[0] && modified_path[modified_path.size() - 1] == new_path[1]) {
                //std::cout << "DEBUG::Player.set_path:: duplicate destinations" << std::endl;
                new_path = std::vector<glm::vec2>(np_2nd_elm_iter + 1, new_path.end());
            }
        };

        // Append all the locations in the given path
        for(auto &pos : new_path) {
            modified_path.push_back(pos);
        }

        // Set the path to the newly modified one
        this->path.set_path(modified_path);
        this->new_path.set_path(modified_path);

        /*
        size_t size = modified_path.size();
        int x_direction = 0;
        if (modified_path[size-1].x != modified_path[0].x) {
            x_direction = (int)(abs(modified_path[size-1].x - modified_path[0].x) / (modified_path[size-1].x - modified_path[0].x));
        }
        int y_direction = 0;
        if (modified_path[size-1].y != modified_path[0].y) {
            y_direction = (int)(abs(modified_path[size-1].y - modified_path[0].y) / (modified_path[size-1].y - modified_path[0].y));
        }

        std::vector<glm::vec2> result_path;

        auto is_in = [modified_path](float x, float y){
            for (auto i : modified_path) {
                if (i.x == x && i.y == y) {
                    return true;
                }
            }
            return false;
        };

        float xx = modified_path[0].x;
        float yy = modified_path[0].y;

        float original_x = modified_path[next_destination_index-1].x;
        float original_y = modified_path[next_destination_index-1].y;

        result_path.emplace_back(glm::vec2(xx, yy));
        while (xx != modified_path[size-1].x || yy != modified_path[size-1].y) {
            if (xx == original_x && yy == original_y) {
                this->next_destination_index = result_path.size();
            }
            if (x_direction != 0 && is_in(xx + x_direction, yy)) {
                result_path.emplace_back(glm::vec2(xx + x_direction, yy));
                xx = xx + x_direction;
                continue;
            } else if(y_direction != 0 && is_in(xx, yy + y_direction)) {
                result_path.emplace_back(glm::vec2(xx, yy + y_direction));
                yy = yy + y_direction;
                continue;
            }
            break;
        }
        */



//         print the new path
        //std::cout << "**== Modified Path ==**" << std::endl;
        //std::cout << next_destination_index << ":" << result_path[next_destination_index].x  << "," << result_path[next_destination_index].y << std::endl;
        //for(auto &pos : result_path) {
        //    std::cout << "\t( " << pos.x << ", " << pos.y << " )" << std::endl;
        //};



        
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