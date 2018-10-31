//
// Created by 何宇 on 2018/10/15.
//

#ifndef MAGPIE_MODEL_H
#define MAGPIE_MODEL_H

#include "Agent.h"

class Model {
public:
//    void draw(Agent* agent ;
    virtual void update(float elapsed, Agent* agent) = 0;
};


#endif //MAGPIE_MODEL_H
