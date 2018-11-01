//
// Created by York on 2018/10/31.
//

#ifndef MAGPIE_GUARDAGENT_H
#define MAGPIE_GUARDAGENT_H

#include "Agent.h"

class GuardAgent : public Agent{
public:
    GuardAgent(int object_id, int group_id);

    void consumeSignal() override;
    void updateState(float elapsed) override;
    void interact() override;

private:
    int object_id;
    int group_id;
    float state_duration = 0.0f;
    float chace_duration = 0.0f;
};


#endif //MAGPIE_GUARDAGENT_H
