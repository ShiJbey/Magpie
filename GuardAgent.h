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
    float chase_duration = 0.0f;
    std::vector<glm::vec2> destinations = {glm::vec2(3, 1), glm::vec2(3, 8), glm::vec2(5, 8), glm::vec2(5, 1)};
    int dest_index = 0;
};


#endif //MAGPIE_GUARDAGENT_H
