#include "Guard.hpp"
#include "../AssetLoader.hpp"

#include <iostream>
#include "GameAgent.hpp"
#include "../game_settings.hpp"

uint32_t Magpie::Guard::instance_count = 0;

Magpie::Guard::Guard() {
    this->instance_id = instance_count;
    instance_count++;

    animation_manager = new AnimationManager();

    facing_direction = DIRECTION::DOWN;
    movespeed = 1.0f;
};

void Magpie::Guard::update(float elapsed, MagpieLevel* level) {
    update_state(elapsed, level);
    animation_manager->update(elapsed);
};

void Magpie::Guard::set_state(uint32_t state) {
    state_duration = 0.0f;
    previous_state = (Guard::STATE) state;
    GameAgent::set_state(state);
    animation_manager->set_current_state(state);

    if (!mute_sound) {
        switch((Guard::STATE) state) {
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
    }
};

void Magpie::Guard::update_state(float elapsed, MagpieLevel* level) {

    state_duration += elapsed;

    // Check the guard's sight
    SIGHT view_state = (SIGHT) check_view(level);

    switch ((STATE) current_state) {
        case STATE::IDLE:
            handle_state_idle(view_state);
            break;
        case STATE::PATROLING:
            handle_state_patrolling(view_state, elapsed);
            break;
        case STATE::CAUTIOUS:
            handle_state_cautious(view_state, elapsed);
            break;
        case STATE::ALERT:
            handle_state_alert(view_state);
            break;
        case STATE::CHASING:
            handle_state_chasing(view_state, elapsed);
            break;
        case STATE::CONFUSED:
            handle_state_confused(view_state);
            break;
        case STATE::EATING:
            handle_state_eating(view_state, level);
        default:
            break;
    }
};

// Base state for all guards doing nothing
// Guards if guards are not stationary (have more than 1 patrol point),
// then they will attempt to leave this state after a brief time
void Magpie::Guard::handle_state_idle(SIGHT view_state) {

    // Magpie has been seen close by
    if (view_state == SIGHT::MAGPIE_ALERT) {
        //std::cout << "IDLE -> ALERT" << std::endl;
        previous_state = STATE::IDLE;
        set_state((uint32_t) STATE::ALERT);
        interest_point = player->get_position();

        // Add a path to the interest poitnt
        std::vector< glm::vec2 > p =
            Magpie::Navigation::getInstance().findPath(
                    glm::vec2(get_position().x, get_position().y),
                    glm::vec2(interest_point.x, interest_point.y)
            );
        append_path(p);
        return;
    }

    // Saw something, but its too far to tell what it is
    if (view_state == SIGHT::MAGPIE_NOTICE) {
        //std::cout << "IDLE -> NOTICE" << std::endl;
        previous_state = STATE::IDLE;
        set_state((uint32_t) STATE::CONFUSED);
        interest_point = player->get_position();
        // Add a path to the interest poitnt
        std::vector< glm::vec2 > p =
            Magpie::Navigation::getInstance().findPath(
                    glm::vec2(get_position().x, get_position().y),
                    glm::vec2(interest_point.x, interest_point.y)
            );
        append_path(p);
        return;
    }

    // Attempt to move to the next patrol point
    if (state_duration > 1.5f) {
        if (patrol_points.size() <= 1) return;

        patrol_point_index = (patrol_point_index + 1) % patrol_points.size();

        glm::vec2 next_patrol_point = patrol_points[patrol_point_index];
        std::vector< glm::vec2 > p =
            Magpie::Navigation::getInstance().findPath(
                    glm::vec2(get_position().x, get_position().y),
                    glm::vec2(next_patrol_point.x, next_patrol_point.y)
            );

        append_path(p);
        previous_state = STATE::IDLE;
        set_state((uint32_t) STATE::PATROLING);
    }
};

// Patroling is the standard walking state. Guards can only
// leave the patroling state when they either:
// 1) Reach the end of their path (PATROL -> IDLE)
// 2) See the Magpie (PATROL -> [ALERT, CAUTIOUS])
void Magpie::Guard::handle_state_patrolling(SIGHT view_state, float elapsed){

    if (view_state == SIGHT::TREAT) {
        previous_state = STATE::CHASING;
        set_state((uint32_t) STATE::EATING);
        return;
    }

    // Magpie has been seen close by
    if (view_state == SIGHT::MAGPIE_ALERT) {
        //std::cout << "PATROL -> ALERT" << std::endl;
        previous_state = STATE::PATROLING;
        set_state((uint32_t) STATE::ALERT);
        interest_point = player->get_position();
        // Add a path to the interest poitnt
        std::vector< glm::vec2 > p =
            Magpie::Navigation::getInstance().findPath(
                    glm::vec2(get_position().x, get_position().y),
                    glm::vec2(interest_point.x, interest_point.y)
            );
        append_path(p);
        return;
    }

    // Saw something, but its too far to tell what it is
    if (view_state == SIGHT::MAGPIE_NOTICE) {
        //std::cout << "PATROL -> CAUTIOUS" << std::endl;
        previous_state = STATE::PATROLING;
        set_state((uint32_t) STATE::CONFUSED);
        interest_point = player->get_position();
        // Add a path to the interest poitnt
        std::vector< glm::vec2 > p =
            Magpie::Navigation::getInstance().findPath(
                    glm::vec2(get_position().x, get_position().y),
                    glm::vec2(interest_point.x, interest_point.y)
            );
        append_path(p);
        return;
    }

    // Continue moving to the next patrol point
    walk(elapsed);

};

// While in the Cautious state, the guard moves towards the interest point
// This state is paired with a slightly altered walking animation
// Guard leave this state by reaching the interest point or by
// spotting the Magpie
void Magpie::Guard::handle_state_cautious(SIGHT view_state, float elapsed) {

    if (view_state == SIGHT::TREAT) {
        previous_state = STATE::CHASING;
        set_state((uint32_t) STATE::EATING);
        return;
    }

    if (view_state == SIGHT::MAGPIE_ALERT) {
        //std::cout << "CAUTIOUS-> ALERT" << std::endl;
        previous_state = STATE::CAUTIOUS;
        set_state((uint32_t) STATE::ALERT);
        interest_point = player->get_position();
        // Add a path to the interest poitnt
        std::vector< glm::vec2 > p =
            Magpie::Navigation::getInstance().findPath(
                    glm::vec2(get_position().x, get_position().y),
                    glm::vec2(interest_point.x, interest_point.y)
            );
        append_path(p);
        return;
    }

    // Move towards the interest point where the guard may have
    // seen something
    walk(elapsed);

}

// When the guard sees the Magpie, it
// enters this state, which is paired with a (!) animation
// This state automatically moves to the CHASING state after
// the animation is complete
void Magpie::Guard::handle_state_alert(SIGHT view_state) {
    if (animation_manager->get_current_animation()->animation_player->done()) {
        //std::cout << "ALERT->CHASING" << std::endl;
        previous_state = STATE::ALERT;
        set_state((uint32_t) STATE::CHASING);
        movespeed = 2.5f;
    }
};

// Chasing is paired with a runnning animation and a higher movespeed
// The guard will stay in this state for a certain period of time, chasing
// the player. If the player is not caught then it moves to idle when reaching
// the end of its path.
void Magpie::Guard::handle_state_chasing(SIGHT view_state, float elapsed) {

    if (view_state == SIGHT::TREAT) {
        previous_state = STATE::CHASING;
        set_state((uint32_t) STATE::EATING);
        return;
    }

    // Continue to update the path to the player's current position
    if (state_duration < 1.5f || view_state == SIGHT::MAGPIE_ALERT) {
        std::vector< glm::vec2 > p =
            Magpie::Navigation::getInstance().findPath(
                    glm::vec2(get_position().x, get_position().y),
                    glm::vec2(player->get_position().x, player->get_position().y)
            );
        append_path(p);
        return;
    }

    // Move along the path to the player's last known position
    walk(elapsed);
};

// When the guard sees something, but is not sure what it saw, it
// enters this state, which is paired with a (?) animation
// This state automatically moves to the CAUTIOUS state after
// the animation is complete
void Magpie::Guard::handle_state_confused(SIGHT view_state) {
    if (state_duration > 1.0f) {
        previous_state = STATE::CONFUSED;
        set_state((uint32_t) STATE::CAUTIOUS);
    }
};

// This handler is coupled with an eating animation and occurs
// when the guard is standing on top of a treat
void Magpie::Guard::handle_state_eating(SIGHT view_state, Magpie::MagpieLevel* level) {
    DogTreat* treat = level->get_treat(glm::round(get_position()));
    if (treat != nullptr) {
        level->set_treat(glm::round(get_position()), nullptr);
        treat->get_scene_object()->active = false;
    }
    if (state_duration > 1.0f) {
        previous_state = STATE::EATING;
        set_state((uint32_t) STATE::IDLE);
    }
};

void Magpie::Guard::turn_to(glm::vec2 loc) {
    glm::vec3 current_position = get_position();

    DIRECTION direction_to_destination = GameAgent::direction_toward(
        glm::vec2(current_position.x, current_position.y),
        loc
    );

    set_facing_direction(direction_to_destination);
    set_model_rotation((uint32_t)direction_to_destination);
};

uint32_t Magpie::Guard::check_view(MagpieLevel* level) {

    // The guard sees nothing if the player is disguised
    if (player->is_disguised()) {
        return (uint32_t)SIGHT::NOTHING;
    }

    // Check if the guard is standing on a donut
    glm::vec2 rounded_position = glm::round(get_position());
    DogTreat* treat = level->get_treat(rounded_position);
    if (treat != nullptr) {
        return (uint32_t) SIGHT::TREAT;
    }

    // Check at various distances in the direction that the guard
    // is facing.
    glm::vec3 vector_to_player = player->get_position() - get_position();
    float distance_to_player = glm::length(vector_to_player);
    float alert_distance = 2.0f;
    float notice_distance = 4.0f;

    if (this->facing_direction == DIRECTION::LEFT && (int)vector_to_player.y == 0) {
        if (distance_to_player <= alert_distance) {
            return (uint32_t) SIGHT::MAGPIE_ALERT;
        }
        else if (distance_to_player <= notice_distance) {
            return (uint32_t) SIGHT::MAGPIE_NOTICE;
        }
    }

    else if (this->facing_direction == DIRECTION::RIGHT
        && (int)vector_to_player.y == 0 && vector_to_player.x < 0) {

        if (abs(distance_to_player) <= alert_distance) {
            return (uint32_t) SIGHT::MAGPIE_ALERT;
        }
        else if (abs(distance_to_player) <= notice_distance) {
            return (uint32_t) SIGHT::MAGPIE_NOTICE;
        }
    }

    else if (this->facing_direction == DIRECTION::UP
        && (int)vector_to_player.x == 0 && vector_to_player.y > 0) {

        if (abs(distance_to_player) <= alert_distance) {
            return (uint32_t) SIGHT::MAGPIE_ALERT;
        }
        else if (abs(distance_to_player) <= notice_distance) {
            return (uint32_t) SIGHT::MAGPIE_NOTICE;
        }
    }
    else if (this->facing_direction == DIRECTION::DOWN
        && (int)vector_to_player.x == 0 && vector_to_player.y < 0) {

        if (abs(distance_to_player) <= alert_distance) {
            return (uint32_t) SIGHT::MAGPIE_ALERT;
        }
        else if (abs(distance_to_player) <= notice_distance) {
            return (uint32_t) SIGHT::MAGPIE_NOTICE;
        }
    }




    // The guard also sees nothing if they just dont see anything
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

void Magpie::Guard::set_model_rotation(uint32_t dir) {
    switch (dir) {
        case (uint32_t)GameAgent::DIRECTION::RIGHT :
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            // std::cout << "DEBUG::Guard::set_model_rotation():: Orienting right" << std::endl;
            break;
        case (uint32_t)GameAgent::DIRECTION::LEFT :
            // std::cout << "DEBUG::Guard::set_model_rotation():: Orienting left" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case (uint32_t)GameAgent::DIRECTION::UP :
            // std::cout << "DEBUG::Guard::set_model_rotation():: Orienting up" << std::endl;
            (*transform)->rotation = original_rotation * glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case (uint32_t)GameAgent::DIRECTION::DOWN:
            // std::cout << "DEBUG::Guard::set_model_rotation():: Orienting down" << std::endl;
            (*transform)->rotation = original_rotation;
            break;
        default:
            std::cout << "ERROR:: Invalid orientation" << std::endl;
            break;
    }
};

void Magpie::Guard::walk(float elapsed) {
    // Check to see if we are at our current_destination
    // If we are, then update the current destination
    // Move towards the current destination at the movement speed

    glm::vec2 current_position_v2  = get_position();
    current_destination = path[path_destination_index];
    float distance_to_destination = glm::length(current_destination - current_position_v2);

    // Stop them from walking off some random place
    if (distance_to_destination > 1) {
        set_position(glm::round(get_position()));
        Guard::set_state((uint32_t)Guard::STATE::IDLE);
        movespeed = 1.0f;
        // Clear the path and resent destination index
        this->path.clear();
        this->path_destination_index = 0;
        return;
    }


    // Consider ourselves to be at the position, snap to it, and return
    if (distance_to_destination < 0.01) {
        set_position(glm::vec3((int)current_destination.x, (int)current_destination.y, 0.0f));
        path_destination_index++;

        // Stop Walking because we are at the end of our path
        if (path_destination_index >= path.size()) {
            Guard::set_state((uint32_t)Guard::STATE::IDLE);
            movespeed = 1.0f;
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
        glm::vec2 elapsed_movement = (glm::length(potential_elapsed_movement) < distance_to_destination) ?
            potential_elapsed_movement : current_destination - current_position_v2;
        set_position(get_position() + glm::vec3(elapsed_movement.x, elapsed_movement.y, 0.0f));
    }
};

void Magpie::Guard::set_position(glm::vec3 position) {
    Magpie::AnimatedModel::set_position(position);
};

void Magpie::Guard::set_starting_point(glm::vec3 position) {
    starting_point = position;
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