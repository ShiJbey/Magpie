//
// Created by York on 2018/10/31.
//

#include "GuardModel.h"

GuardModel::GuardModel(Scene::Transform *transform) {
    this->transform = transform;
    this->rotation = transform->rotation;
}

void GuardModel::update(float elapsed, Agent *agent) {
    transform->position.x = agent->position.x;
    transform->position.y = agent->position.y;

    float angle = 0.0f;
    angle = agent->orientation * 90.0f;

    transform->rotation = this->rotation * glm::angleAxis(glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));
}

