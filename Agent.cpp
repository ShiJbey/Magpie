//
// Created by 何宇 on 2018/10/15.
//

#include "Agent.h"

glm::vec2 Agent::getDirectionVec2() {
    switch (orientation) {
        case UP:
            return glm::vec2(0, -1);
        case RIGHT:
            return glm::vec2(1, 0);
        case DOWN:
            return glm::vec2(0, 1);
        case LEFT:
            return glm::vec2(-1, 0);
        default:
            return glm::vec2(0, 0);
    }
}

void Agent::turnTo(glm::vec2 destination) {
    int x_direction = sgn(destination.x - coordinate.x);
    int y_direction = sgn(destination.y - coordinate.y);

    if (x_direction == 0) {
        if (y_direction == 1) {
            orientation = DOWN;
        } else {
            orientation = UP;
        }
    }

    if (y_direction == 0) {
        if (x_direction == 1) {
            orientation = RIGHT;
        } else {
            orientation = LEFT;
        }
    }
}