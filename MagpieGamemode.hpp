#pragma once

#include "Mode.hpp"
#include "TransformAnimation.hpp"
#include "Grid.h"
#include "Navigation.h"

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

        //update position is used to update the magpie or guard's position with pathfinder
        void updatePosition(char character, std::vector<glm::uvec2> path);

        //mouse pick sends a raycast from where the mouse has clicked and returns which tile
        //the user has landed in
        glm::uvec2 mousePick(int mouseX, int mouseY, int screenWidth, int screenHeight,
                                int floorHeight, const Scene::Camera* cam, std::string floorPlan);

        //given a point of intersection, tileMap tries to find a matching tile in given floorplan 
        //and returns that. if not it returns -1, -1
        glm::uvec2 tileMap(glm::vec3 isect, std::string floorPlan);

        Scene::Transform *player_transform = nullptr;
        /*
        std::vector< Scene::Transform *> player_model_walk_transforms;
        std::vector< Scene::Transform *> player_model_idle_transforms;
        std::vector< Scene::Transform *> player_model_steal_transforms;
        std::list< TransformAnimationPlayer > current_animations;
        */

        Grid currFloor;
        float magMoveCountdown = 5.0f;
        glm::uvec2 magpie = glm::uvec2(0, 0);
        glm::uvec2 magpieEndpt = glm::uvec2(0, 0);
        std::vector<glm::uvec2> magpieWalkPath;
    };
}
