//
// Created by 何宇 on 2018/10/15.
//

#ifndef MAGPIE_MODEL_H
#define MAGPIE_MODEL_H

#include <glm/gtc/quaternion.hpp>
#include "Agent.h"

class Model {
public:
//    void draw(Agent* agent ;
    virtual void update(float elapsed, Agent* agent) = 0;
    glm::quat rotation;
};


#endif //MAGPIE_MODEL_H
