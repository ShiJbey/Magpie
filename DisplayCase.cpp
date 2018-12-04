#include "DisplayCase.hpp"
#include <iostream>

float Magpie::DisplayCase::required_lock_pick_time = 1.0f;
uint32_t Magpie::DisplayCase::instance_count = 0;

Magpie::DisplayCase::DisplayCase() {
    this->instance_id = instance_count;
    instance_count++;
    this->opened = false;
};

Magpie::BoundingBox* Magpie::DisplayCase::get_boundingbox() {
    if (this->bounding_box != nullptr) return this->bounding_box;
    else if (this->scene_object != nullptr) {
        this->bounding_box = new BoundingBox(this->scene_object->transform->position,
            glm::vec3(0.5f, 0.5f, 1.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
    }
    else if (this->get_transform() != nullptr) {
        this->bounding_box = new BoundingBox(this->get_position(),
            glm::vec3(0.5f, 0.5f, 1.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
    }
    return this->bounding_box;
};

void Magpie::DisplayCase::on_click() {
    //std::cout << "Display Case Clicked" << std::endl;
    glass->active = false;
    frame->active = false;
    opened = true;
    (*transform)->rotation*= glm::angleAxis(glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0));
};

Scene::Transform* Magpie::DisplayCase::load_model(Scene& scene, const Magpie::ModelData* model_data, std::string model_name,
    std::function< Scene::Object*(Scene &, Scene::Transform *, std::string const &) > const &on_object) {

    std::vector< std::string > model_parts = { "_GRP", "glass_", "frame_" };

    Scene::Transform* model_group_transform = nullptr;

    bool model_group_found = false;

    std::vector< Scene::Transform * > hierarchy_transforms;
    hierarchy_transforms.reserve(model_data->hierarchy.size());

    for (auto const &h : model_data->hierarchy) {
        Scene::Transform *t = scene.new_transform();
        if (h.parent != -1U) {
            if (h.parent >= hierarchy_transforms.size()) {
                throw std::runtime_error("Model data did not contain transforms in topological-sort order.");
            }
            t->set_parent(hierarchy_transforms[h.parent]);
        }

        if (h.name_begin <= h.name_end && h.name_end <= model_data->names.size()) {
            // NOTE:: Here we need to do some checks on the transform names to ensure
            //      Transforms are named differently

            // Get the name from the char vector
            std::string exported_name = std::string(model_data->names.begin() + h.name_begin, model_data->names.begin() + h.name_end);

            if (exported_name.find(model_name) != std::string::npos) {
                // Check if the name contains any of the model part string
                for (auto part: model_parts) {
                    if (exported_name.find(part) != std::string::npos) {
                        // We have a name match, now set a new name with the instance ID
                        // (e.g. Magpie_body_0)
                        if (part.compare("_GRP") == 0) {
                            t->name = model_name + "_GRP_" + std::to_string(instance_id);

                            if (exported_name.find(model_name) != std::string::npos && !model_group_found) {
                                model_group_found = true;
                                model_group_transform = t;
                            }
                            else if (!model_group_found) {
                                model_group_transform = t;
                            }
                            
                        } else {
                            t->name = model_name + "_" + part + "_" + std::to_string(instance_id);
                        }
                        break;                      
                    }
                }
            }

            

        } else {
                throw std::runtime_error("Model data contains hierarchy entry with invalid name indices");
        }

        t->position = h.position;
        t->rotation = h.rotation;
        t->scale = h.scale;

        hierarchy_transforms.emplace_back(t);
    }

    assert(hierarchy_transforms.size() == model_data->hierarchy.size());

    for (auto const &m : model_data->meshes) {
        if (m.transform >= hierarchy_transforms.size()) {
            throw std::runtime_error("Model data contains mesh entry with invalid transform index (" + std::to_string(m.transform) + ")");
        }
        if (!(m.name_begin <= m.name_end && m.name_end <= model_data->names.size())) {
            throw std::runtime_error("Model data contains mesh entry with invalid name indices");
        }
        std::string name = std::string(model_data->names.begin() + m.name_begin, model_data->names.begin() + m.name_end);

        if (on_object) {
            Scene::Object *obj = on_object(scene, hierarchy_transforms[m.transform], name);
                if (obj != nullptr) {
                    if (obj->transform->name.find("frame") == std::string::npos) {
                    this->frame = obj;
                }
                else if (obj->transform->name.find("glass") == std::string::npos) {
                    this->glass = obj;
                }
            }
        }
    }
    
    return model_group_transform;
};