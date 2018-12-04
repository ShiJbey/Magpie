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
    walk(elapsed);
    update_state(elapsed);
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

    if (glm::distance(get_position(), player->get_position()) < 0.1) {
        player->game_over = true;
    }

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
        std::cout << "IDLE -> ALERT" << std::endl;
        set_state((uint32_t) STATE::ALERT);
        interest_point = player->get_position();
        return;
    }

    if (view_state == SIGHT::MAGPIE_NOTICE) {
        std::cout << "IDLE -> NOTICE" << std::endl;
        set_state((uint32_t) STATE::CAUTIOUS);
        interest_point = player->get_position();
        return;
    }

    if (state_duration > 1.5f) {
        if (patrol_points.size() <= 1) return;
        patrol_index = (patrol_index + 1) % patrol_points.size();
        set_destination(patrol_points[patrol_index]);
        set_state((uint32_t) STATE::PATROLING);
    }

}

void Magpie::Guard::handle_state_patrolling(enum SIGHT view_state){
    if (view_state == SIGHT::MAGPIE_ALERT) {
        std::cout << "PATROL -> ALERT" << std::endl;
        set_state((uint32_t) STATE::ALERT);
        interest_point = player->get_position();
        return;
    }

    if (view_state == SIGHT::MAGPIE_NOTICE && !cautious) {
        std::cout << "PATROL -> CAUTIOUS" << std::endl;
        cautious = true;
        set_state((uint32_t) STATE::CAUTIOUS);
        interest_point = player->get_position();
        return;
    }
};

void Magpie::Guard::handle_state_cautious(enum SIGHT view_state) {
    if (view_state == SIGHT::MAGPIE_ALERT) {
        std::cout << "CAUTIOUS-> ALERT" << std::endl;
        set_state((uint32_t) STATE::ALERT);
        interest_point = player->get_position();
        return;
    }
}

void Magpie::Guard::handle_state_alert(enum SIGHT view_state) {
    if (state_duration > 1.5f) {
        std::cout << "ALERT->CHASING" << std::endl;
        set_state((uint32_t) STATE::CHASING);
        Magpie::Guard::set_destination(glm::vec2(player->get_position()));
    }
}

void Magpie::Guard::handle_state_chasing(enum SIGHT view_state) {
//    if (view_state == SIGHT::NOTHING) {
//        std::cout << "CHASING -> CONFUSED" << std::endl;
//        set_state((uint32_t) STATE::CONFUSED);
//        return;
//    }
    if (state_duration > 1.5f) {
        set_state((uint32_t) STATE::CHASING);
        set_destination(player->get_position());
    }
}

void Magpie::Guard::handle_state_confused(enum SIGHT view_state) {

    if (view_state == SIGHT::MAGPIE_ALERT) {
        std::cout << "CONFUSED -> ALERT" << std::endl;
        set_state((uint32_t) STATE::ALERT);
        interest_point = player->get_position();
        set_destination(interest_point);
        return;
    }

    if (view_state == SIGHT::MAGPIE_NOTICE) {
        std::cout << "CONFUSED -> NOTICE" << std::endl;
        set_state((uint32_t) STATE::CAUTIOUS);
        interest_point = player->get_position();
        set_destination(interest_point);
        return;
    }

    if (state_duration > 4.0f) {
//        patrol_index = 0;
//        set_destination(patrol_points[patrol_index]);
//        set_state((uint32_t) STATE::PATROLING);
        cautious = false;
    }
}

uint32_t Magpie::Guard::check_view() {
    if (player->is_disguised()) return (uint32_t)SIGHT::NOTHING;

    glm::vec3 v = glm::vec3(board_position.x, board_position.y, 0) - player->get_position();

    glm::vec3 o_v3;

    switch (orientation) {
        case DIRECTION::DOWN:
            o_v3 = glm::vec3(-1.0, -1.0, 0.0);
            break;
        case DIRECTION::UP:
            o_v3 = glm::vec3(1.0, 1.0, 0.0);
            break;
        case DIRECTION::LEFT:
            o_v3 = glm::vec3(-1.0, 1.0, 0.0);
            break;
        case DIRECTION::RIGHT:
            o_v3 = glm::vec3(1.0, -1.0, 0.0);
            break;
    }

    glm::vec3 vv = v * o_v3;


    float x = -vv.x;
    float y = -vv.y;

    if (orientation == DIRECTION::LEFT || orientation == DIRECTION::RIGHT) {
        y = abs(y);

        float t = x;
        x = y;
        y = t;
    }

    if (orientation == DIRECTION::UP || orientation == DIRECTION::DOWN) {
        x = abs(x);
    }
//    std::cout << "O:" << (uint32_t)orientation <<  " o_v:" << o_v3.x << "," << o_v3.y << " v:" << x << "," << y << " vv:" << vv.x << "," << vv.y << std::endl;
//    std::cout << (uint32_t)orientation << "  " << current_state << ", " << interest_point.x << "," << interest_point.y << std::endl;

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

    uint32_t count = 0;
    result.push_back(p);
    glm::vec2 old_dir = glm::vec2(-1, -1);
    while (count < points.size()) {
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

    this->patrol_points = result;
}


void Magpie::Guard::interact() {

}

void Magpie::Guard::set_model_orientation(uint32_t dir) {
   switch(dir) {
        case (uint32_t)GameAgent::DIRECTION::RIGHT :
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//           std::cout << "DEBUG:: Orienting right" << std::endl;
            break;
        case (uint32_t)GameAgent::DIRECTION::LEFT :
//            std::cout << "DEBUG:: Orienting left" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case (uint32_t)GameAgent::DIRECTION::UP :
//            std::cout << "DEBUG:: Orienting up" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case (uint32_t)GameAgent::DIRECTION::DOWN:
//            std::cout << "DEBUG:: Orienting down" << std::endl;
            (*transform)->rotation = original_rotation;
            break;
        default:
            std::cout << "ERROR:: Invalid orientation" << std::endl;
            break;
    }
}

void Magpie::Guard::walk(float elapsed) {

    if (
            current_state == (uint32_t)STATE::IDLE ||
            current_state == (uint32_t)STATE::ALERT ||
            current_state == (uint32_t)STATE::CONFUSED
    ) return;

    if (is_new_path) {
        std::cout << "NEW PATH" << std::endl;
        is_new_path = false;
        path = new_path;

        next_destination_index = 0;

		glm::vec3 next_destination = glm::vec3(path.get_path()[next_destination_index], 0.0f);
        next_destination_index++;
        current_destination = glm::vec3(next_destination.x, next_destination.y, 0.0f);

        accumulate_time = 0;
		Magpie::Guard::turnTo(current_destination);
		Magpie::Guard::set_model_orientation((uint32_t)orientation);
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

    movespeed = 1.0f;

    if (current_state == (uint32_t)STATE::CHASING) movespeed = 2.5f;

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
//            printf("\tCurrent Destination: (%f, %f, %f)\n", current_destination.x, current_destination.y, current_destination.z);
//            printf("\tNext Destination Index is now: %d\n", next_destination_index);
            Magpie::Guard::turnTo(current_destination);
            Magpie::Guard::set_model_orientation((uint32_t)orientation);
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
    std::cout << "Guard Set Destination to" << destination.x << ", " << destination.y << std::endl;

    if (destination == last_destination) return;
    last_destination = destination;

    Path p =
    Magpie::Navigation::getInstance().findPath(
            glm::vec2(get_position().x, get_position().y),
            glm::vec2(destination.x, destination.y)
    );

    Magpie::GameAgent::set_path(p);

    std::cout << "Set Path" << this->path.get_path().size() << std::endl;

   if (this->path.get_path().size() > 0) {
    } else {
//        set_state((uint32_t) STATE::IDLE);
    }
}

void Magpie::Guard::turnTo(glm::vec3 destination) {

    float x_difference = destination.x - get_position().x;
    x_difference = -x_difference;
    float y_difference = destination.y - get_position().y;
    y_difference = -y_difference;

    // WARNING::Only handles movement in cardinal directions
    if (x_difference > 0) {
//        std::cout << "DEBUG:: Facing right" << std::endl;
        orientation = DIRECTION::LEFT;
    }
    else if (x_difference < 0) {
//        std::cout << "DEBUG:: Facing left" << std::endl;
        orientation = DIRECTION::RIGHT;
    }
    else {
        if (y_difference > 0) {
            std::cout << "DEBUG:: Facing down" << std::endl;
            orientation = DIRECTION::DOWN;
        } else {
            std::cout << "DEBUG:: Facing up" << std::endl;
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