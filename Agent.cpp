//
// Created by 何宇 on 2018/10/15.
//

#include "Agent.h"

glm::vec2 Agent::getDirectionVec2() {
    switch (orientation) {
        case UP:
            return glm::vec2(0, 1);
        case RIGHT:
            return glm::vec2(-1, 0);
        case DOWN:
            return glm::vec2(0, -1);
        case LEFT:
            return glm::vec2(1, 0);
        default:
            return glm::vec2(0, 0);
    }
}

void Agent::turnTo(glm::uvec2 destination) {
    std::cout << "TURN TO" << destination.x << "," << destination.y << std::endl;
    int x_direction = sgn(destination.x - position.x);
    int y_direction = sgn(destination.y - position.y);

    if (x_direction == 0) {
        if (y_direction == 1) {
            orientation = UP;
        } else {
            orientation = DOWN;
        }
    }

    if (y_direction == 0) {
        if (x_direction == 1) {
            orientation = LEFT;
        } else {
            orientation = RIGHT;
        }
    }
}

void Agent::setDestination(glm::uvec2 destination){

    state = WALKING;
    std::cout << "Destination Set to " << destination.x << "," << destination.y << std::endl;

    this->path = Navigation::getInstance().findPath(position, destination);

    cur_destination = this->path.top();
}

void Agent::walk(float elapsed) {

    if (state == IDLE) return;

    float distance = elapsed * velocity;
    std::cout << "Walking to " << cur_destination.x << "," << cur_destination.y << "from" << position.x << ","
            << position.y << "Direction" << orientation << ":" << getDirectionVec2().x << "," << getDirectionVec2().y << std::endl;

    glm::vec2 vector_to =  cur_destination - position;

    if (glm::length(vector_to) < distance || glm::dot(vector_to, getDirectionVec2()) < 0) {
        position = cur_destination;
        if (path.isEmpty()) {
            state = IDLE;
        } else {
            cur_destination = path.next();
            turnTo(cur_destination);
        }
    } else {
        position += getDirectionVec2() * distance;
    }
}

void Agent::update(float elapsed) {
    // First, each agent will consume some signal from the signal box (if any);
    consumeSignal();

    // Second, the agent will be moved;
    walk(elapsed);

    // Third, the agent will update its state;
    updateState(elapsed);

    // Finally, the agent will check what it can interact with (which will produce signals).
    // For example, the guard will check if it sees the magpie.
    interact();
}

