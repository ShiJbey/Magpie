#pragma once

#include "Mode.hpp"
#include "Grid.h"
#include "Navigation.h"
#include "Scene.hpp"

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

	//initialize a list of random items to steal
	void initHitList();

	//checks for game win/loss condition. returns a std::vector where first element
	//is whether or not the game is over, the second is win or loss
	std::vector<bool> gameOver();

	//handle_event is called when new mouse or keyboard events are received:
	// (note that this might be many times per frame or never)
	//The function should return 'true' if it handled the event.
	virtual bool handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) override;

	//update is called at the start of a new frame, after events are handled:
	virtual void update(float elapsed) override;

	//update position is used to update the magpie or guard's position with pathfinder
	void updatePosition(char character, std::vector<glm::vec2> path);

	//update the direction in which the characters are facing as they move automatically
	char dirFaced(glm::vec2 currPos, glm::vec2 nextPos);

	//mouse pick sends a raycast from where the mouse has clicked and returns which tile
	//the user has landed in
	glm::vec2 mousePick(int mouseX, int mouseY, int screenWidth, int screenHeight,
							   int floorHeight, const Scene::Camera* cam, std::string floorPlan);

	//given a point of intersection, tileMap tries to find a matching tile in given floorplan 
	//and returns that. if not it returns -1, -1
	glm::vec2 tileMap(glm::vec3 isect, std::string floorPlan);

	//steal item sequence (plays magpie steal animation, deletes item from scene)
	void stealItem(glm::vec2 itemPos);

	//draw is called after update:
	virtual void draw(glm::uvec2 const &drawable_size) override;

	Grid currFloor;
	std::vector<std::string> allItems; // all items on map
	uint32_t numItems = 5; //number of items player has to grab to win
	std::vector<std::string> hitlist;
	//Navigation magpieNav;
	float camera_spin = 0.0f;
	float spot_spin = 0.0f;
	float magMoveCountdown = 5.0f;
	char magpieDir = 'U'; //TODO: CHECK OUT ENUM
	glm::vec2 guard = glm::vec2(6, 6);
	glm::vec2 magpie = glm::vec2(0, 0);
	glm::vec2 magpieEndpt = glm::vec2(0, 0);
	std::vector<glm::vec2> magpieWalkPath;
	
};
