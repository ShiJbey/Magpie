//
// Created by York on 2018/10/31.
//

#include "GuardAgent.h"
#include "SignalQueue.h"
#include "MagpieGame.hpp"

GuardAgent::GuardAgent(int object_id, int group_id) {
    this->position.x = 0;
    this->position.y = 3;
    this->orientation = DOWN;
    this->object_id = object_id;
    this->group_id = group_id;
}

void GuardAgent::consumeSignal() {
    Signal* s = nullptr;

    while ((s = SignalQueue::getInstance().get(object_id, group_id)) != nullptr) {
    }
}

void GuardAgent::updateState(float elapsed) {
    state_duration += elapsed;
    if (state == CHASING) {
        // The guard will chase the player for 20 seconds, then go back to his original point.
        chace_duration += elapsed;
        if (state_duration > 2000.0f) {
            std::cout << "TIRED" << std::endl;
            state = RETURN;
        } else {
            if (chace_duration > 50.0f) {
                setDestination(Magpie::game.player->getPosition(), CHASING);
                std::cout << "Destination Set" << " STATE " << state << std::endl;
                chace_duration = 0;
            }
        }
    }
}

void GuardAgent::interact() {
    glm::vec2 player_position = Magpie::game.player->getPosition();

    glm::vec2 distance = position - player_position;

//    std::cout << "(" << distance.x << "," << distance.y << ")" << std::endl;
    if (orientation == RIGHT) {
        if (distance.x >= -2 && distance.x < 0 && distance.y >= -1 && distance.y <= 1) {
            std::cout << "START CHASING" << std::endl;
            state = CHASING;
            chace_duration = 100.0f;
        }
    }

    if (orientation == LEFT) {
        if (distance.x <= 2 && distance.x > 0 && distance.y >= -1 && distance.y <= 1) {
            std::cout << "START CHASING" << std::endl;
            state = CHASING;
            chace_duration = 100.0f;
        }
    }

    if (orientation == DOWN) {
        if (distance.x >= -1 && distance.x <= 1 && distance.y > 0 && distance.y <= 2) {
            std::cout << "START CHASING" << std::endl;
            state = CHASING;
            chace_duration = 100.0f;
        }
    }

    if (orientation == UP) {
        if (distance.x >= -1 && distance.x <= 1 && distance.y < 0 && distance.y >= -2) {
            std::cout << "START CHASING" << std::endl;
            state = CHASING;
            chace_duration = 100.0f;
        }
    }

    if (abs(distance.x) < 0.5 && abs(distance.y) < 0.5) {
        state = CAUGHT;
        return;
    }
}