#pragma once

#include "../base/Mode.hpp"
#include "../base/TransformAnimation.hpp"
#include "../base/MeshBuffer.hpp"
#include "../base/GL.hpp"
#include "../base/Scene.hpp"

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

// Simple Mode that displays models playing animations
// Mainly used for debuging animation functionality
struct TestAnimationMode : public Mode {
    TestAnimationMode();
    virtual ~TestAnimationMode();

    //handle_event is called when new mouse or keyboard events are received:
	// (note that this might be many times per frame or never)
	//The function should return 'true' if it handled the event.
	virtual bool handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) override;

	//update is called at the start of a new frame, after events are handled:
	virtual void update(float elapsed) override;

	//draw is called after update:
	virtual void draw(glm::uvec2 const &drawable_size) override;

    Scene *scene;
    Scene::Camera *camera = nullptr;
	std::vector< Scene::Transform *> player_model_walk_transforms;
	std::vector< Scene::Transform *> player_model_idle_transforms;
	std::vector< Scene::Transform *> player_model_steal_transforms;
	std::list< TransformAnimationPlayer > current_animations;
};