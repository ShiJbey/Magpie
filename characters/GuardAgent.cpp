//
// Created by York on 2018/10/31.
//

#include "GuardAgent.h"
#include "../MagpieGame.hpp"

GuardAgent::GuardAgent(int object_id, int group_id) {
    this->position.x = 1;
    this->position.y = 7;
    this->orientation = DOWN;
    this->object_id = object_id;
    this->group_id = group_id;
    this->state = WALKING;
    this->dest_index = 0;
    this->velocity = 1.5f;
    setDestination(destinations[dest_index]);
}

void GuardAgent::consumeSignal() {
    Signal* s = nullptr;

    while ((s = SignalQueue::getInstance().get(object_id, group_id)) != nullptr) {
    }
}

void GuardAgent::updateState(float elapsed) {
    //std::cout << "STATE:" << state << "," << state_duration << std::endl;
    state_duration += elapsed;
    if (state == CHASING) {
        // The guard will chase the player for 20 seconds, then go back to his original point.
        chase_duration += elapsed;
        if (state_duration > 20.0f) {
            //std::cout << "TIRED" << std::endl;
            state = WALKING;
            dest_index = 0;
            state_duration = 0;
            setDestination(destinations[0]);
        } else {
            if (chase_duration > 0.5f || path.isEmpty())  {
                glm::vec2 p = Magpie::game.player->getPosition();
                setDestination(p, CHASING);
                //std::cout << "Destination Set" << p.x << "," << p.y <<" STATE " << state << std::endl;
                chase_duration = 0;
            }
        }
    } else if (state == WALKING) {
        glm::vec2 distance = destinations[dest_index] - position;

        if (glm::length(distance) < 0.1) {
            position = destinations[dest_index];
            dest_index = (dest_index + 1) % 4;
            setDestination(destinations[dest_index]);
        }
    } else if (state == IDLE) {
        state = WALKING;
        dest_index = 0;
        setDestination(destinations[0]);
    }
}

void GuardAgent::interact() {
    glm::vec2 player_position = Magpie::game.player->getPosition();

    glm::vec2 distance = position - player_position;

//    std::cout << orientation << "(" << distance.x << "," << distance.y << ")" << std::endl;
    if (orientation == RIGHT) {
        if (distance.x >= -3 && distance.x < 0 && distance.y >= -1 && distance.y <= 1) {
            //std::cout << "START CHASING" << std::endl;
            state = CHASING;
            state_duration = 0;
            chase_duration = 0;
        }
    }

    if (orientation == LEFT) {
        if (distance.x <= 3 && distance.x > 0 && distance.y >= -1 && distance.y <= 1) {
            //std::cout << "START CHASING" << std::endl;
            state = CHASING;
            state_duration = 0;
            chase_duration = 0;
        }
    }

    if (orientation == DOWN) {
        if (distance.x >= -1 && distance.x <= 1 && distance.y > 0 && distance.y <= 3) {
            //std::cout << "START CHASING" << std::endl;
            state = CHASING;
            state_duration = 0;
            chase_duration = 0;
        }
    }

    if (orientation == UP) {
        if (distance.x >= -1 && distance.x <= 1 && distance.y < 0 && distance.y >= -3) {
            //std::cout << "START CHASING" << std::endl;
            state = CHASING;
            state_duration = 0;
            chase_duration = 0;
        }
    }

    if (abs(distance.x) < 0.5 && abs(distance.y) < 0.5) {
        state = CAUGHT;
        SignalQueue::getInstance().send(new Signal("caught", 0, 0));
        return;
    }
}