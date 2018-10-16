//
// Created by 何宇 on 2018/10/15.
//

#ifndef MAGPIE_AGENT_H
#define MAGPIE_AGENT_H


#include <cassert>
#include <glm/glm.hpp>


template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

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

    glm::vec2 getDirectionVec2();
    void turnTo(glm::vec2 destination);

    DIRECTION orientation;
    glm::vec2 coordinate;
};


#endif //MAGPIE_AGENT_H
