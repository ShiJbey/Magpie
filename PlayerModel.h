//
// Created by York on 2018/10/31.
//

#ifndef MAGPIE_PLAYERMODEL_H
#define MAGPIE_PLAYERMODEL_H

#include "Model.h"
#include "Scene.hpp"


class PlayerModel : public Model {
public:
    PlayerModel(Scene::Transform* transform);

    void update(float elapsed, Agent* agent) override;

private:
    Scene::Transform* transform;
};


#endif //MAGPIE_PLAYERMODEL_H
