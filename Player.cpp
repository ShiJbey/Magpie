#include "Player.hpp"

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

void Magpie::Player::setDestination(glm::vec3 destination) {
    current_destination = destination;
    //set_velocity(glm::normalize(destination - get_position()));
    Player::turnTo(current_destination);
    set_model_orientation(orientation);
};

void Magpie::Player::walk(float elapsed) {

    if (is_new_path) {
        
        is_new_path = false;
        path = new_path;

        next_destination_index = 0;

        printf("=====Starting on a new path=====\n");
        printf("\tBeginning Destination Index: %d\n", next_destination_index);

		glm::vec3 next_destination = glm::vec3(path.get_path()[next_destination_index], 0.0f);
        next_destination_index++;
        current_destination = glm::vec3(next_destination.x, next_destination.y, 0.0f);

        printf("\t#### Destination Set ####\n");
        printf("\tCurrent Destination: (%f, %f, %f)\n", current_destination.x, current_destination.y, current_destination.z);
        printf("\tNext Destination Index is now: %d\n", next_destination_index);

        accumulate_time = 0;
		turnTo(current_destination);
		set_model_orientation(orientation);
		return;
    }

//    printf("Positions in path: %zd\n", path.get_path().size());
   
//    for (int i = 0; i < path.get_path().size(); i++) {
//        printf("(%f, %f),", path.get_path()[i].x, path.get_path()[i].y);
//    }
//    printf("\n");

    //float distance = elapsed * movespeed;

    //glm::vec2 displacement = getDirectionVec2() * distance;

	if (next_destination_index > this->path.get_path().size())
	{
		Player::set_state((uint32_t)Player::STATE::IDLE);
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

//    

	glm::vec2 moving_distance = accumulate_time * vector_to * movespeed;
//    printf("Moving Distance: (%f, %f, %f, %f, %f)\n", accumulate_time, vector_to.x, vector_to.y, moving_distance.x, moving_distance.y);

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

        printf("\t#### Destination Set ####\n");
            printf("\tCurrent Destination: (%f, %f, %f)\n", current_destination.x, current_destination.y, current_destination.z);
            printf("\tNext Destination Index is now: %d\n", next_destination_index);
            turnTo(current_destination);
            set_model_orientation(orientation);
		
        
	}

	//printf("Current Destination: (%f, %f, %f)\n", current_destination.x, current_destination.y, current_destination.z);
    //printf("Current Position: (%f, %f, %f)\n", get_position().x, get_position().y, get_position().z);
    Player::set_position(current_position);
    


	/*
    //if (glm::length(vector_to) < distance || glm::dot(vector_to, glm::vec3(getDirectionVec2().x, getDirectionVec2().y, 0.0f)) < 0) {

        if (next_destination_index == path.get_path().size()) {

            Player::set_state((uint32_t)Player::STATE::IDLE);
            next_destination_index = 0;
            return;

        } else {
            
            //Player::set_position(current_destination);
           
            printf("Player Position: (%f, %f, %f\n", get_position().x, get_position().y, get_position().z);
            //printf("DESTINATION REACHED\n");
            
            glm::vec2 next_destination = path.get_path()[next_destination_index];
            next_destination_index++;
            
            current_destination = glm::vec3(next_destination.x, next_destination.y, 0.0f);
            Player::set_position(current_destination);
            printf("NEXT DESTINATION: ( %f, %f)\n", current_destination.x, current_destination.y);

            //turnTo(current_destination);

            //set_model_orientation(orientation);

            

            

        }

    //} else {

        //Player::set_position((*transform)->position + glm::vec3(displacement, 0.0f));

    //}

    set_position(get_position() + elapsed * velocity * movespeed);

    if (glm::length(current_destination - get_position()) <= 0.01) {
        set_position(current_destination);
        set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
        set_state((uint32_t)Player::STATE::IDLE);
    }
	*/
};

void Magpie::Player::consume_signal() {

};

void Magpie::Player::update(float elapsed) {
    animation_manager->update(elapsed);
    
    
    if (current_state == (uint32_t)Player::STATE::WALKING) {
        walk(elapsed);
    }

    if (current_state == (uint32_t)Player::STATE::STEALING && animation_manager->get_current_animation()->animation_player->done()) {
        set_state((uint32_t)Player::STATE::IDLE);
        set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
    }
};

void Magpie::Player::update_state(float elapsed) {

};

void Magpie::Player::interact() {

};

void Magpie::Player::set_position(glm::vec3 position) {
    Magpie::GameCharacter::set_position(position);
    board_position = glm::ivec3((int)position.x, (int)position.y, 0);
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
    if (x_difference > 0) {
        std::cout << "DEBUG:: Facing right" << std::endl;
        orientation = DIRECTION::RIGHT;
    }
    else if (x_difference < 0) {
        std::cout << "DEBUG:: Facing left" << std::endl;
        orientation = DIRECTION::LEFT;
    }
    else {
        if (y_difference > 0) {
            std::cout << "DEBUG:: Facing up" << std::endl;
            orientation = DIRECTION::UP;
        } else {
            std::cout << "DEBUG:: Facing down" << std::endl;
            orientation = DIRECTION::DOWN;
        }
    }
};
