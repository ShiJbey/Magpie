//
// Created by York on 2018/10/31.
//

#ifndef MAGPIE_GUARDMODEL_H
#define MAGPIE_GUARDMODEL_H

#include "Model.h"
#include "Scene.hpp"

class GuardModel : public Model{
public:
    GuardModel(Scene::Transform* transform);

    void update(float elapsed, Agent* agent) override;

private:
    Scene::Transform* transform;
};


#endif //MAGPIE_GUARDMODEL_H
