#include "Guard.hpp"
#include "AssetLoader.hpp"

#include <iostream>
#include "GameAgent.hpp"

uint32_t Magpie::Guard::instance_count = 0;

Magpie::Guard::Guard() {
    this->instance_id = instance_count;
    instance_count++;

    animation_manager = new AnimationManager();
    orientation = DIRECTION::LEFT;
    movespeed = 2.0f;
};

void Magpie::Guard::consume_signal() {

};

void Magpie::Guard::turnTo(glm::vec3 destination) {
    
    float x_difference = destination.x - get_position().x;
    float y_difference = destination.y - get_position().y;

    // WARNING::Only handles movement in cardinal directions
    if (x_difference > 0) {
        //std::cout << "DEBUG:: Facing right" << std::endl;
        orientation = DIRECTION::RIGHT;
    }
    else if (x_difference < 0) {
        //std::cout << "DEBUG:: Facing left" << std::endl;
        orientation = DIRECTION::LEFT;
    }
    else {
        if (y_difference > 0) {
            //std::cout << "DEBUG:: Facing up" << std::endl;
            orientation = DIRECTION::UP;
        } else {
            //std::cout << "DEBUG:: Facing down" << std::endl;
            orientation = DIRECTION::DOWN;
        }
    }
};

void Magpie::Guard::update(float elapsed) {
    if (current_state == (uint32_t)STATE::PATROLING || current_state == (uint32_t)STATE::CHASING || current_state == (uint32_t)STATE::CAUTIOUS)
        walk(elapsed);
    update_state(elapsed);
    animation_manager->update(elapsed);
};


void Magpie::Guard::set_state(uint32_t state) {
    state_duration = 0.0f;
    last_state = (enum STATE) state;
    GameAgent::set_state(state);
    animation_manager->set_current_state(state);

    switch((enum STATE) state) {
        case STATE::CAUTIOUS:
            sample_guard_cautious->play(get_position());
            break;
        case STATE::ALERT:
            sample_guard_alert->play(get_position());
            break;
        case STATE::CONFUSED:
            sample_guard_confused->play(get_position());
            break;
        default:
            break;
    }
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
        if (debug_focus)
            std::cout << "Guard (" << get_instance_id() << ") " << "IDLE -> ALERT" << std::endl;
        set_state((uint32_t) STATE::ALERT);
        return;
    }

    if (view_state == SIGHT::MAGPIE_NOTICE) {
        if (debug_focus)
            std::cout << "Guard (" << get_instance_id() << ") " << "IDLE -> ALERT" << std::endl;
        set_state((uint32_t) STATE::ALERT);
        cautious = true;
        return;
    }

    // WARNING:: This may cause weird behavior when stationary guards return to their positions
    // after chasing
    if (state_duration > 1.5f) {
        if (patrol_points.size() > 1) {
            patrol_index = (patrol_index + 1) % patrol_points.size();
            set_destination(patrol_points[patrol_index]);
            set_state((uint32_t) STATE::PATROLING);
        }
        else if (get_position().x != patrol_points[0].x && get_position().y != patrol_points[0].y) {
            set_destination(patrol_points[0]);
            set_state((uint32_t) STATE::PATROLING);
        }
    }

}

void Magpie::Guard::handle_state_patrolling(enum SIGHT view_state){
    if (view_state == SIGHT::MAGPIE_ALERT) {
        if (debug_focus)
            std::cout << "Guard (" << get_instance_id() << ") " << "PATROL -> ALERT" << std::endl;
        set_state((uint32_t) STATE::ALERT);
        interest_point = player->get_position();
        return;
    }

    if (view_state == SIGHT::MAGPIE_NOTICE && !cautious) {
        if (debug_focus)
            std::cout << "Guard (" << get_instance_id() << ") " << "PATROL -> CAUTIOUS" << std::endl;
        cautious = true;
        set_state((uint32_t) STATE::CAUTIOUS);
        return;
    }

    if (view_state == SIGHT::DONUT) {
        if (debug_focus)
            std::cout << "Guard (" << get_instance_id() << ") " << "PATROL -> EATING" << std::endl;
        cautious = false;
        set_state((uint32_t) STATE::EATING);
        return;
    }


};

void Magpie::Guard::handle_state_cautious(enum SIGHT view_state) {
    
    if (view_state == SIGHT::MAGPIE_ALERT) {
        if (debug_focus)
            std::cout << "Guard (" << get_instance_id() << ") " << "CAUTIOUS-> ALERT" << std::endl;
        set_state((uint32_t) STATE::ALERT);
        cautious = false;
        interest_point = player->get_position();
        return;
    }

    if (state_duration > 3.0f) {
        cautious = false;
        std::cout << "Guard (" << get_instance_id() << ") " << "CAUTIOUS-> PATROL" << std::endl;
        set_state((uint32_t)STATE::PATROLING);
    }
    
}

void Magpie::Guard::handle_state_alert(enum SIGHT view_state) {
    if (current_state == (uint32_t)Guard::STATE::ALERT && animation_manager->get_current_animation()->animation_player->done()) {
        if (cautious) {
            if (patrol_points.size() == 1) {
                if (debug_focus)
                    std::cout << "Guard (" << get_instance_id() << ") " << "ALERT->CHASING (cautious stationary)" << std::endl;
                
                set_state((uint32_t) STATE::CHASING);
                Magpie::Guard::set_destination(glm::vec2(player->get_position().x, player->get_position().y));
            }
            else {
                if (debug_focus)
                    std::cout << "Guard (" << get_instance_id() << ") " << "ALERT->CAUTIOUS" << std::endl;
                set_state((uint32_t) STATE::CAUTIOUS);
            }
        }
        else {
            if (debug_focus)
                    std::cout << "Guard (" << get_instance_id() << ") " << "ALERT->CHASING" << std::endl;
                
            set_state((uint32_t) STATE::CHASING);
            Magpie::Guard::set_destination(glm::vec2(player->get_position().x, player->get_position().y));
        }
    }
};

void Magpie::Guard::handle_state_chasing(enum SIGHT view_state) {
    switch(view_state) {
        case SIGHT::DONUT:
            std::cout << "Guard (" << get_instance_id() << ") " << "CHASE -> EAT" << std::endl;
            set_state((uint32_t)STATE::EATING);
            break;
        case SIGHT::NOTHING:
            if (state_duration > 1.0f) {
                std::cout << "Guard (" << get_instance_id() << ") " << "CHASE -> CONFUSED" << std::endl;
                set_state((uint32_t)STATE::CONFUSED);
            } else {
                set_destination(player->get_position());
            }
            break;
        default:
            set_destination(player->get_position());
            state_duration = 0;
            break;
    }
};

void Magpie::Guard::handle_state_confused(enum SIGHT view_state) {
    if (current_state == (uint32_t)STATE::CONFUSED && animation_manager->get_current_animation()->animation_player->done()) {
       set_state((uint32_t)STATE::CAUTIOUS);
       cautious = true;
       set_destination(patrol_points[patrol_index]);
    }
};

uint32_t Magpie::Guard::check_view() {
    // Check for any donuts

    for (auto const &treat : *dog_treats_in_level) {
        glm::vec3 distance_to_treat = treat->transform->position = get_position();
            switch (orientation) {
            case DIRECTION::DOWN:
                if (distance_to_treat.y < 0 && glm::length(distance_to_treat) <= 2) {
                    return (uint32_t) SIGHT::DONUT;
                }
                break;
        case DIRECTION::UP:
                if (distance_to_treat.y > 0 && glm::length(distance_to_treat) <= 2) {
                    return (uint32_t) SIGHT::DONUT;
                }
            break;
        case DIRECTION::LEFT:
                if (distance_to_treat.x < 0 && glm::length(distance_to_treat) <= 2) {
                    return (uint32_t) SIGHT::DONUT;
                }
            break;
        case DIRECTION::RIGHT:
                if (distance_to_treat.x > 0 && glm::length(distance_to_treat) <= 2) {
                    return (uint32_t) SIGHT::DONUT;
                }
            break;
        }
        
    }

    // Check the state of the player
    if (player->is_disguised()) return (uint32_t)SIGHT::NOTHING;

    glm::vec3 distance_to_player = player->get_position() - get_position();

    // Now we check if the magpie falls in range based on the orientation
    // of the guard
    switch (orientation) {
        case DIRECTION::DOWN:
            if (distance_to_player.y < 0 && glm::length(distance_to_player) <= 2) {
                return (uint32_t) SIGHT::MAGPIE_ALERT;
            }
            else if (distance_to_player.y < 0 && glm::length(distance_to_player) <= 3) {
                return (uint32_t) SIGHT::MAGPIE_NOTICE;
            }
            break;
       case DIRECTION::UP:
            if (distance_to_player.y > 0 && glm::length(distance_to_player) <= 2) {
                return (uint32_t) SIGHT::MAGPIE_ALERT;
            }
            else if (distance_to_player.y > 0 && glm::length(distance_to_player) <= 3) {
                return (uint32_t) SIGHT::MAGPIE_NOTICE;
            }
           break;
       case DIRECTION::LEFT:
            if (distance_to_player.x < 0 && glm::length(distance_to_player) <= 2) {
                return (uint32_t) SIGHT::MAGPIE_ALERT;
            }
            else if (distance_to_player.x < 0 && glm::length(distance_to_player) <= 3) {
                return (uint32_t) SIGHT::MAGPIE_NOTICE;
            }
           break;
       case DIRECTION::RIGHT:
            if (distance_to_player.x > 0 && glm::length(distance_to_player) <= 2) {
                return (uint32_t) SIGHT::MAGPIE_ALERT;
            }
            else if (distance_to_player.x > 0 && glm::length(distance_to_player) <= 3) {
                return (uint32_t) SIGHT::MAGPIE_NOTICE;
            }
           break;
    }

    return (uint32_t)SIGHT::NOTHING;
};

bool find(const std::vector<glm::vec2>& points, glm::vec2 p) {
    for (auto i : points) {
        if (i == p) {
            return true;
        }
    }
    return false;
};

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
};

void Magpie::Guard::interact() {

};

void Magpie::Guard::set_model_orientation(uint32_t dir) {
   switch(dir) {
        case (uint32_t)GameAgent::DIRECTION::RIGHT :
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//           std::cout << "DEBUG::Guard::set_model_orientation():: Orienting right" << std::endl;
            break;
        case (uint32_t)GameAgent::DIRECTION::LEFT :
//            std::cout << "DEBUG::Guard::set_model_orientation():: Orienting left" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case (uint32_t)GameAgent::DIRECTION::UP :
//            std::cout << "DEBUG::Guard::set_model_orientation():: Orienting up" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case (uint32_t)GameAgent::DIRECTION::DOWN:
//            std::cout << "DEBUG::Guard::set_model_orientation():: Orienting down" << std::endl;
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
        //std::cout << "NEW PATH" << std::endl;
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
};

void Magpie::Guard::set_destination(glm::vec2 destination) {
    //if (debug_focus)
    //    std::cout << "Guard Set Destination to" << destination.x << ", " << destination.y << std::endl;

    if (destination == last_destination) return;
    last_destination = destination;

    Path p =
    Magpie::Navigation::getInstance().findPath(
            glm::vec2(get_position().x, get_position().y),
            glm::vec2(destination.x, destination.y)
    );

    set_path(p);

    //if (debug_focus)
        //std::cout << "Set Path" << this->path.get_path().size() << std::endl;

    if (this->path.get_path().size() > 0) {

    }
    else {
        //set_state((uint32_t) STATE::IDLE);
    }
};

void Magpie::Guard::set_path(Magpie::Path path) {

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
        //glm::vec3 rounded_position = glm::round(get_position());

        // Erase all locations after the next destination
        modified_path.erase(modified_path.begin() + next_destination_index, modified_path.end());

        // Remove overlapping destinations
        if (modified_path.size() >= 2 && new_path.size() >= 2) {
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
    }
};

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

Magpie::BoundingBox* Magpie::Guard::get_boundingbox() {
    if (bounding_box != nullptr) {
        delete bounding_box;
        bounding_box = nullptr;
    }
    this->bounding_box = new BoundingBox(get_position(), glm::vec3(0.5f, 0.5f, 1.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
    return this->bounding_box;
};

void Magpie::Guard::on_click() {
    this->debug_focus = true;
};