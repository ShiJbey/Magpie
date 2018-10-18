//
// Created by 何宇 on 2018/10/15.
//

#ifndef MAGPIE_GUARDAGENT_H
#define MAGPIE_GUARDAGENT_H

#include "Agent.h"
#include "Navigation.h"

#include <vector>
#include <algorithm>

class GuardAgent : Agent {

    GuardAgent() = default;
    GuardAgent(Agent* player);

    void update(float elapsed) override;

private:
    std::vector<glm::vec2> route_point;
    std::vector<glm::vec2>::iterator destination = route_point.end();
    Path* path;
    float speed = 1;

    Agent* player;
};


#endif //MAGPIE_GUARDAGENT_H
