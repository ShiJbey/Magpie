//
// Created by 何宇 on 2018/10/15.
//

#include "GuardAgent.h"


void GuardAgent::update(float elapsed) {
    if (destination == route_point.end()) {
        destination = route_point.begin();
        path = Navigation::getInstance().findPath(coordinate, *destination);
    }

    turnTo(*destination);

    glm::vec2 walk_distance = speed * elapsed * getDirectionVec2();

    walk_distance.x = sgn(walk_distance.x) * std::min(
            std::abs(walk_distance.x),
            std::abs(destination->x - coordinate.x)
    );

    walk_distance.y = sgn(walk_distance.y) * std::min(
            std::abs(walk_distance.y),
            std::abs(destination->y - coordinate.y)
    );

    coordinate += walk_distance;

    if (coordinate == *destination) {
        destination++;
    }

    //TODO: Sight view detection
    if (glm::distance(player->coordinate, coordinate) < 0.1) {
        //TODO: Game over
    }
}
