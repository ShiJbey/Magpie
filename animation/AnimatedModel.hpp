#pragma once

#include "../base/Scene.hpp"
#include "AnimationManager.hpp"
#include "TransformAnimation.hpp"

#include <vector>
#include <functional>

namespace Magpie {

    /**
     * Borrowed from Scene::load(...)
     */
    struct HierarchyEntry {
		uint32_t parent;
		uint32_t name_begin;
		uint32_t name_end;
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};

    /**
     * Also borrowed from Scene::load(...)
     */
    struct MeshEntry {
		uint32_t transform;
		uint32_t name_begin;
		uint32_t name_end;
	};

    /**
     * This struct holds the transform hierarchy data that is necessary to create instances
     * of models such as characters (e.g. player and guards)
     */
    struct ModelData {
        std::vector< char > names;
        std::vector< HierarchyEntry > hierarchy;
        std::vector< MeshEntry > meshes;

        ModelData(std::string const &filename);
    };

    /**
     * This class is used by all objects in the scene that
     * need to be animated. This includes doors, keys, paintings,
     * or anything else that moves.
     */
    class AnimatedModel {
    public:

        AnimatedModel();
        ~AnimatedModel();

        // Imports a character model from a file
        virtual Scene::Transform* load_model(Scene& scene, const ModelData* model_data, std::string model_name,
            std::function< Scene::Object* (Scene &, Scene::Transform *, std::string const &) > const &on_object);

        // Converts the names imported from the animation t-anim
        virtual std::vector< std::string > convert_animation_names(const TransformAnimation* tanim, std::string model_name);

        // SETTERS
        void set_position(glm::vec3 position);
        void set_transform(Scene::Transform** transform);
        void set_animation_manager(AnimationManager* animation_manager);
        virtual void set_model_orientation(uint32_t dir);

        // GETTERS
        glm::vec3 get_position();
        Scene::Transform** get_transform();
        AnimationManager* get_animation_manager();
        uint32_t get_instance_id();

    protected:
        // Pointer to the, transform pointer for this character
        // Note:: the transform is managed by the animation manager
        Scene::Transform** transform;

        // Manages the current model of the player
        // as well as all associated animations
        AnimationManager* animation_manager;

        // Stores the original rotation of the magpie model.
        // This is used when rotating the player transform
        glm::quat original_rotation;

        // Secific identifier for which intance this is
        // NOTE:: Instance counts are managed by specific
        //      classes, not by this class. For example,
        //      Guard has a different count than Player
        uint32_t instance_id;

    };

}