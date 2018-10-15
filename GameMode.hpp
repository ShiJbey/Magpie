#pragma once

#include "Mode.hpp"
#include "Grid.h"

#include "MeshBuffer.hpp"
#include "GL.hpp"

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

// The 'GameMode' mode is the main gameplay mode:

struct GameMode : public Mode {
	GameMode();
	virtual ~GameMode();

	//handle_event is called when new mouse or keyboard events are received:
	// (note that this might be many times per frame or never)
	//The function should return 'true' if it handled the event.
	virtual bool handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) override;

	//update is called at the start of a new frame, after events are handled:
	virtual void update(float elapsed) override;

	//update position is used to update the magpie or guard's position with pathfinder
	void updatePosition(char character, std::vector<glm::uvec2> path);

	//mouse pick sends a raycast from where the mouse has clicked and returns which tile
	//the user has landed in
	glm::uvec2 mousePick(int mouseX, int mouseY, );

	//draw is called after update:
	virtual void draw(glm::uvec2 const &drawable_size) override;

	float camera_spin = 0.0f;
	float spot_spin = 0.0f;
	float magMoveCountdown = 5.0f;
	glm::uvec2 magpie = glm::uvec2(0, 0);
	glm::uvec2 magpieEndpt = glm::uvec2(0, 0);
	
};
