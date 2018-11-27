#include "Guard.hpp"

#include <iostream>

uint32_t Magpie::Guard::instance_count = 0;

Magpie::Guard::Guard() {
    this->instance_id = instance_count;
    instance_count++;

    animation_manager = new AnimationManager();
};

void Magpie::Guard::consume_signal() {

};

void Magpie::Guard::update(float elapsed) {
    update_state(elapsed);
    walk(elapsed);
    animation_manager->update(elapsed);
};


void Magpie::Guard::set_state(uint32_t state) {
    state_duration = 0.0f;
    last_state = (enum STATE) state;
    GameAgent::set_state(state);
    animation_manager->set_current_state(state);
};

void Magpie::Guard::update_state(float elapsed) {

    state_duration += elapsed;

    if (check_view()) return;

    switch ((enum STATE) current_state) {
        case STATE::IDLE:
            handle_state_idle();
            break;
        case STATE::PATROLING:
            handle_state_patrolling();
            break;
        case STATE::CAUTIOUS:
            handle_state_cautious();
            break;
        case STATE::ALERT:
            handle_state_alert();
            break;
        case STATE::CHASING:
            handle_state_chasing();
            break;
        case STATE::CONFUSED:
            handle_state_confused();
            break;
        default:
            break;
    }
};

// IDLE State
void Magpie::Guard::handle_state_idle() {
    if (state_duration > 1.5f) {
        patrol_index = (patrol_index + 1) % patrol_points.size();
        std::cout << "NEXT PATROL POINT " << patrol_index << " (" << patrol_points[patrol_index].x << "," << patrol_points[patrol_index].y << ")" << std::endl;
        setDestination(patrol_points[patrol_index]);
        std::cout << "IDLE1" << std::endl;
        set_state((uint32_t) STATE::PATROLING);
        std::cout << "IDLE" << std::endl;
    }

}

void Magpie::Guard::handle_state_patrolling(){
    float distance = glm::length(current_destination - get_position());

    if (distance < 0.1) {
//        set_state((uint32_t) STATE::IDLE);
    }
};

void Magpie::Guard::handle_state_cautious() {

}

void Magpie::Guard::handle_state_alert() {

}

void Magpie::Guard::handle_state_chasing() {

}

void Magpie::Guard::handle_state_confused() {

}

bool Magpie::Guard::check_view() {
    return false;
}

void Magpie::Guard::set_patrol_points(std::vector<glm::vec3> points) {
    this->patrol_points = points;
}

void Magpie::Guard::interact() {

}


void Magpie::Guard::walk(float elapsed) {

    if (current_state == (uint32_t)STATE::IDLE) return;

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

	if (next_destination_index > this->path.get_path().size())
	{
		set_state((uint32_t)STATE::IDLE);
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

        printf("\t#### Destination Set ####\n");
            printf("\tCurrent Destination: (%f, %f, %f)\n", current_destination.x, current_destination.y, current_destination.z);
            printf("\tNext Destination Index is now: %d\n", next_destination_index);
            turnTo(current_destination);
            set_model_orientation(orientation);
	}

    set_position(current_position);
};

void Magpie::Guard::set_position(glm::vec3 position) {
    Magpie::GameCharacter::set_position(position);
    board_position = glm::ivec3((int)position.x, (int)position.y, 0);
}

void Magpie::Guard::setDestination(glm::vec3 destination) {
    std::cout << "Set Destination to" << destination.x << ", " << destination.y << std::endl;
    set_path(Magpie::Navigation::getInstance().findPath(
            glm::vec2(get_position().x, get_position().y),
            glm::vec2(destination.x, destination.y)
    ));

    std::cout << "Set Path" << destination.x << ", " << destination.y << std::endl;

    is_new_path = true;
    //TODO:
    set_state((uint32_t)STATE::PATROLING);
}

void Magpie::Guard::turnTo(glm::vec3 destination) {

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
}

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