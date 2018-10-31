//
// Created by York on 2018/10/31.
//

#ifndef MAGPIE_PLAYERAGENT_H
#define MAGPIE_PLAYERAGENT_H

#include "Agent.h"

class PlayerAgent : public Agent {
public:

    PlayerAgent(int object_id, int group_id);

    void consumeSignal() override;
    void updateState(float elapsed) override;
    void interact() override;

};


#endif //MAGPIE_PLAYERAGENT_H
