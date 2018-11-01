//
// Created by 何宇 on 2018/10/15.
//

#ifndef MAGPIE_ENTITY_H
#define MAGPIE_ENTITY_H

#include "Model.h"
#include "Agent.h"

/**
 * class Entity is the top-level class of all interactable objects in the game;
 * including Player Character, NPC, Traps and treasures.
 */


class Entity {
public:
    Entity()= default;
    Entity(Model* model, Agent* agent);
    void update(float elapsed);
    void draw();
    void setDestination(glm::uvec2 destination);

    Agent* getAgent() {
        return agent;
    }

    glm::vec2 getPosition();

protected:
    Model* model = nullptr;
    Agent* agent = nullptr;
};


#endif //MAGPIE_ENTITY_H
