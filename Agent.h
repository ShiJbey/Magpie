//
// Created by 何宇 on 2018/10/15.
//

#ifndef MAGPIE_AGENT_H
#define MAGPIE_AGENT_H


#include <cassert>
#include <glm/glm.hpp>

class Agent {
public:
    Agent()= default;
    virtual void update(float elapsed){
        assert(false);
    };

    enum DIRECTION {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };

    DIRECTION orientation;
    glm::vec2 coordinate;
};


#endif //MAGPIE_AGENT_H
