//
// Created by 何宇 on 2018/10/15.
//

#ifndef MAGPIE_AGENT_H
#define MAGPIE_AGENT_H


#include <cassert>
#include <glm/glm.hpp>

#include "Navigation.h"


template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

class Agent {
public:
    Agent()= default;
    void update(float elapsed);

    enum DIRECTION {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };

    glm::vec2 getDirectionVec2();
    void turnTo(glm::uvec2 destination);
    void setDestination(glm::uvec2 destination);
    void walk(float elapsed);

    // Virtual functions for inherited classes to implement
    virtual void consumeSignal() = 0;
    virtual void updateState(float elapsed) = 0;
    virtual void interact() = 0;

    float velocity = 2.0f;

    DIRECTION orientation;
    glm::vec2 position;
    glm::vec2 cur_destination;
    Path path;

    enum STATE {
        WALKING,
        IDLE
    };

    enum STATE state = IDLE;
};


#endif //MAGPIE_AGENT_H
