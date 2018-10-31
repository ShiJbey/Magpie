//
// Created by York on 2018/10/31.
//

#include "PlayerModel.h"

PlayerModel::PlayerModel(Scene::Transform *transform) {
    this->transform = transform;
}

void PlayerModel::update(float elapsed, Agent* agent) {
    transform->position.x = agent->position.x + 0.5f;
    transform->position.y = agent->position.y + 0.5f;
}

