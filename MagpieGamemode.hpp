#pragma once

#include "MagpieGame.hpp"
#include "MagpieLevel.hpp"
#include "Navigation.h"

#include "Mode.hpp"
#include "TransformAnimation.hpp"
#include "MeshBuffer.hpp"
#include "GL.hpp"
#include "Scene.hpp"

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

namespace Magpie {
    // Game mode for playing the MagpieGame
    struct MagpieGameMode : public Mode {

        MagpieGameMode();
        virtual ~MagpieGameMode();

        //handle_event is called when new mouse or keyboard events are received:
        // (note that this might be many times per frame or never)
        //The function should return 'true' if it handled the event.
        virtual bool handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) override;

        //update is called at the start of a new frame, after events are handled:
        virtual void update(float elapsed) override;

        //draw is called after update:
        virtual void draw(glm::uvec2 const &drawable_size) override;

        //mouse pick sends a raycast from where the mouse has clicked and returns which tile
        //the user has landed in
        glm::uvec2 mousePick(int mouseX, int mouseY, int screenWidth, int screenHeight,
                                int floorHeight, const Scene::Camera* cam);

        //given a point of intersection, tileMap tries to find a matching tile in given floorplan 
        //and returns that. if not it returns -1, -1
        glm::uvec2 tileMap(glm::vec3 isect, std::string floorPlan);

        // Set up the scene using the level loader and such
        void init_scene();

        Scene scene;
    };
}
