#include "Guard.hpp"

#include <iostream>
#include "GameAgent.hpp"

uint32_t Magpie::Guard::instance_count = 0;

Magpie::Guard::Guard() {
    this->instance_id = instance_count;
    instance_count++;

    animation_manager = new AnimationManager();
    orientation = DIRECTION::LEFT;
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

    // Check the guard's sight
    enum SIGHT view_state = (enum SIGHT) check_view();

    switch ((enum STATE) current_state) {
        case STATE::IDLE:
            handle_state_idle(view_state);
            break;
        case STATE::PATROLING:
            handle_state_patrolling(view_state);
            break;
        case STATE::CAUTIOUS:
            handle_state_cautious(view_state);
            break;
        case STATE::ALERT:
            handle_state_alert(view_state);
            break;
        case STATE::CHASING:
            handle_state_chasing(view_state);
            break;
        case STATE::CONFUSED:
            handle_state_confused(view_state);
            break;
        default:
            break;
    }
};

// IDLE State
void Magpie::Guard::handle_state_idle(enum SIGHT view_state) {

    cautious = false;

    if (view_state == SIGHT::MAGPIE_ALERT) {
        set_state((uint32_t) STATE::ALERT);
        interest_point = player->get_position();
        return;
    }

    if (view_state == SIGHT::MAGPIE_NOTICE) {
        set_state((uint32_t) STATE::CAUTIOUS);
        interest_point = player->get_position();
        return;
    }

    if (state_duration > 1.5f) {
        if (patrol_points.size() <= 1) return;
        std::cout << "size" << patrol_points.size() << std::endl;
        patrol_index = (patrol_index + 1) % patrol_points.size();
        std::cout << "NEXT PATROL POINT " << patrol_index << " (" << patrol_points[patrol_index].x << "," << patrol_points[patrol_index].y << ")" << std::endl;
        set_destination(patrol_points[patrol_index]);
        std::cout << "IDLE1" << std::endl;
        set_state((uint32_t) STATE::PATROLING);
        std::cout << "IDLE" << std::endl;
    }

}

void Magpie::Guard::handle_state_patrolling(enum SIGHT view_state){
    if (view_state == SIGHT::MAGPIE_ALERT) {
        set_state((uint32_t) STATE::ALERT);
        interest_point = player->get_position();
        return;
    }

    if (view_state == SIGHT::MAGPIE_NOTICE && !cautious) {
        cautious = true;
        set_state((uint32_t) STATE::CAUTIOUS);
        interest_point = player->get_position();
        return;
    }
};

void Magpie::Guard::handle_state_cautious(enum SIGHT view_state) {
    if (view_state == SIGHT::MAGPIE_ALERT) {
        set_state((uint32_t) STATE::ALERT);
        interest_point = player->get_position();
        return;
    }

    if (state_duration > 1.5f) {
        set_destination(interest_point);
        set_state((uint32_t) STATE::PATROLING);
    }
}

void Magpie::Guard::handle_state_alert(enum SIGHT view_state) {
    if (state_duration > 1.5f) {
        set_destination(interest_point);
        set_state((uint32_t) STATE::CHASING);
    }
}

void Magpie::Guard::handle_state_chasing(enum SIGHT view_state) {
    if (view_state != SIGHT::NOTHING) {
        set_state((uint32_t) STATE::CONFUSED);
        return;
    }
}

void Magpie::Guard::handle_state_confused(enum SIGHT view_state) {

    if (view_state == SIGHT::MAGPIE_ALERT) {
        set_state((uint32_t) STATE::ALERT);
        interest_point = player->get_position();
        return;
    }

    if (view_state == SIGHT::MAGPIE_NOTICE) {
        set_state((uint32_t) STATE::CAUTIOUS);
        interest_point = player->get_position();
        return;
    }

    if (state_duration > 4.0f) {
        patrol_index = 0;
        set_destination(patrol_points[patrol_index]);
        set_state((uint32_t) STATE::PATROLING);
        cautious = false;
    }
}

uint32_t Magpie::Guard::check_view() {
    return (uint32_t)SIGHT::NOTHING;

    glm::vec3 v = glm::vec3(board_position.x, board_position.y, 0) - player->get_position();

    glm::vec3 o_v3;

    switch (orientation) {
        case DIRECTION::UP:
            o_v3 = glm::vec3(-1.0, -1.0, 0.0);
            break;
        case DIRECTION::DOWN:
            o_v3 = glm::vec3(1.0, 1.0, 0.0);
            break;
        case DIRECTION::RIGHT:
            o_v3 = glm::vec3(-1.0, 1.0, 0.0);
            break;
        case DIRECTION::LEFT:
            o_v3 = glm::vec3(1.0, -1.0, 0.0);
            break;
    }

    glm::vec3 vv = v * o_v3;


    float x = vv.x;
    float y = vv.y;

    if (orientation == DIRECTION::UP || orientation == DIRECTION::DOWN) {
        x = abs(x);
    }

    if (orientation == DIRECTION::LEFT || orientation == DIRECTION::RIGHT) {
        y = abs(y);

        float t = x;
        x = y;
        y = t;
    }

//    std::cout << "O:" << (uint32_t)orientation <<  " o_v:" << o_v3.x << "," << o_v3.y << " v:" << x << "," << y << " vv:" << vv.x << "," << vv.y << std::endl;

    if (x <= 2 && y < 4 && y > 2) {
        return (uint32_t) SIGHT::MAGPIE_NOTICE;
    }

    if (x <= 2 && y <= 2 && y > 0) {
        return (uint32_t) SIGHT::MAGPIE_ALERT;
    }

    return (uint32_t)SIGHT::NOTHING;
}

bool find(const std::vector<glm::vec2>& points, glm::vec2 p) {
    for (auto i : points) {
        if (i == p) {
            return true;
        }
    }
    return false;
}

void Magpie::Guard::set_patrol_points(std::vector<glm::vec2> points) {

    glm::vec2 p = glm::vec2(starting_point.x, starting_point.y);
    std::vector<glm::vec2> result;

    glm::vec2 dir[4] = {
            glm::vec2(0, 1),
            glm::vec2(1, 0),
            glm::vec2(0, -1),
            glm::vec2(-1, 0)
    };

    int count = 0;
    result.push_back(p);
    glm::vec2 old_dir = glm::vec2(-1, -1);
    while (count < points.size()) {
        std::cout << "x:" << p.x << " y:" << p.y << std::endl;
        count += 1;
        for (int i = 0; i < 4; i++) {
            // Won't turn back
            if (old_dir + dir[i] == glm::vec2(0,0)) continue;

            if (find(points, p + dir[i])) {

                if (old_dir == glm::vec2(-1, -1)) {
                    old_dir = dir[i];
                    p = p + dir[i];
                    break;
                }

                if (dir[i] != old_dir) {
                    std::cout << "turn x:" << p.x << " y:" << p.y << std::endl;
                    old_dir = dir[i];
                    if (result[0] != p) result.push_back(p);
                    p = p + dir[i];
                    break;
                }

                p = p + dir[i];
            }
        }
    }

    if (result.size() == 1 && result[0] != p) result.push_back(p);

    std::cout << "RESULT" << std::endl;
    for (auto i : result) {
        std::cout << "x:" << i.x << " y:" << i.y << std::endl;
    }

    std::cout << std::endl;

    this->patrol_points = result;
}


void Magpie::Guard::interact() {

}


void Magpie::Guard::walk(float elapsed) {

    if (
            current_state == (uint32_t)STATE::IDLE ||
            current_state == (uint32_t)STATE::CAUTIOUS ||
            current_state == (uint32_t)STATE::ALERT ||
            current_state == (uint32_t)STATE::CONFUSED
    ) return;

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
		set_model_orientation((uint32_t)orientation);
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
            set_model_orientation((uint32_t)orientation);
	}

    set_position(current_position);
};

void Magpie::Guard::set_position(glm::vec3 position) {
    Magpie::AnimatedModel::set_position(position);
    board_position = glm::ivec3((int)position.x, (int)position.y, 0);
}

void Magpie::Guard::set_starting_point(glm::vec3 position) {
    starting_point = position;
}

void Magpie::Guard::set_destination(glm::vec2 destination) {
    std::cout << "Set Destination to" << destination.x << ", " << destination.y << std::endl;
    Path path =
    Magpie::Navigation::getInstance().findPath(
            glm::vec2(get_position().x, get_position().y),
            glm::vec2(destination.x, destination.y)
    );

    set_path(path);

    std::cout << "Set Path" << destination.x << ", " << destination.y << std::endl;

    if (path.get_path().size() > 0) {
        is_new_path = true;
        //TODO:
        set_state((uint32_t) STATE::PATROLING);
    } else {
        set_state((uint32_t) STATE::IDLE);
    }
}

void Magpie::Guard::turnTo(glm::vec3 destination) {

    float x_difference = destination.x - get_position().x;
    float y_difference = destination.y - get_position().y;

    // WARNING::Only handles movement in cardinal directions
    if (x_difference > 0) {
        std::cout << "DEBUG:: Facing right" << std::endl;
        orientation = DIRECTION::LEFT;
    }
    else if (x_difference < 0) {
        std::cout << "DEBUG:: Facing left" << std::endl;
        orientation = DIRECTION::RIGHT;
    }
    else {
        if (y_difference > 0) {
            std::cout << "DEBUG:: Facing up" << std::endl;
            orientation = DIRECTION::DOWN;
        } else {
            std::cout << "DEBUG:: Facing down" << std::endl;
            orientation = DIRECTION::UP;
        }
    }
}

/**
 * Loads a character model identically to how it imports scenes, 
 * however it modifies transform names to prevent, duplicate names
 */
Scene::Transform* Magpie::Guard::load_model(Scene& scene, const ModelData* model_data, std::string model_name,
    std::function< Scene::Object* (Scene &, Scene::Transform *, std::string const &) > const &on_object) {

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