#pragma once

#include "Scene.hpp"
#include "AnimationManager.hpp"
#include "GameAgent.hpp"

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
     * Inherited by all characters in the game that have a model
     * associated with them (e.g. the player and the guard)
     */
    class GameCharacter {
    public:

        GameCharacter();
        ~GameCharacter();

        // Imports a character model from a file
        virtual void load_character_model(Scene& scene, const ModelData* model_data, std::function< void(Scene &, Scene::Transform *, std::string const &) > const &on_object);
        
        // SETTERS
        void set_position(glm::vec3 position);
        void set_transform(Scene::Transform** transform);
        void set_velocity(glm::vec3 velocity);
        void set_model_orientation(GameAgent::DIRECTION dir);

        // GETTERS
        glm::vec3 get_position();
        Scene::Transform** get_transform();
        AnimationManager* get_animation_manager();

    protected:
        // Pointer to the, transform pointer for this character
        // Note:: the transform is managed by the animation manager
        Scene::Transform** transform;
        
        // Manages the current model of the player
        // as well as all associated animations
        AnimationManager* animation_manager;

        // Velocity is used when updating the movement of the character
        glm::vec3 velocity;

        // Stores the original rotation of the magpie model.
        // This is used when rotating the player transform
        glm::quat original_rotation;

    };

}