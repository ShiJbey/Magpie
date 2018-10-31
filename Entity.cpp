//
// Created by 何宇 on 2018/10/15.
//

#include "Entity.h"

Entity::Entity(Model *model, Agent *agent) {
    this->model = model;
    this->agent = agent;
}

void Entity::update(float elapsed) {
    agent->update(elapsed);
    model->update(elapsed, agent);
}

void Entity::draw() {
//    model->draw(agent);
}

void Entity::setDestination(glm::uvec2 destination) {
    agent->setDestination(destination);
}