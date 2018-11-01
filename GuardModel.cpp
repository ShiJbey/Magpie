//
// Created by York on 2018/10/31.
//

#include "GuardModel.h"

GuardModel::GuardModel(Scene::Transform *transform) {
    this->transform = transform;
}

void GuardModel::update(float elapsed, Agent *agent) {
    transform->position.x = agent->position.x;
    transform->position.y = agent->position.y;
}

