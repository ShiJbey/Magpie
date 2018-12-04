#pragma once

#include "MagpieGame.hpp"
#include "MagpieLevel.hpp"
#include "Navigation.h"
#include "load_level.hpp"

#include "UI.h"

#include "Mode.hpp"
#include "TransformAnimation.hpp"
#include "MeshBuffer.hpp"
#include "GL.hpp"
#include "Scene.hpp"

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <unordered_map>

namespace Magpie {

    // Rays are used when performing ray casts when the player
    // clicks anywhere on the game window
    struct Ray {
        Ray() : Ray(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)) {}
        Ray(glm::vec3 origin, glm::vec3 direction) {
            this->origin = origin;
            this->direction = direction;
        }
        glm::vec3 origin;
        glm::vec3 direction;
    };

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

        //default game start menu
        //void open_start_menu();

        ////////////////////////////////////////////////
        //        Magpie Specific Functions           //
        ////////////////////////////////////////////////

        // Makes the walls to the left and below the given position
        // transparent
        void make_close_walls_transparent(float x, float y);

        // Loads a level from a file, instantiates all the
        // meshes and positions the camera
        void load_level(const LevelData *level_data);
        void setup_camera();

        // Adds a new guard to the game by loading all the model data
        // and animations. Then it sets attributes for what programs to use
        Guard* create_guard(glm::vec3 position);

        // Adds a new player to the game by loading all the model data
        // and animations. Then it sets attributes for what programs to use
        Player* create_player(glm::vec3 position);

        // Places a "treat" at the players current position
        // treats distract the guards while they are walking
        Item* drop_treat(glm::vec3 position);
      
        // Highlights the tile meshes along the player's path
        // NOTE:: This function will most likely be deleted since
        //        we are moving to dota style movement.
        void highlight_path_tiles();

        // Handles all screen clicks by delegating athourity to either
        // handle_clickables() or handle_player_movement()
        bool handle_screen_click(Magpie::Ray click_ray);

        // Set play movement to the destination calculated by a raycast
        // from the players screen click
        bool handle_player_movement(glm::vec3 click_floor_intersect);

        // Performs collision detection between the given ray
        // and Clickable objects in the worlds
        bool handle_clickables(Magpie::Ray click_ray);

        //mouse pick sends a raycast from where the mouse has clicked and returns which tile
        //the user has landed in
        glm::vec3 get_click_floor_intersect(Magpie::Ray click_ray, float floorHeight);

        // Creates a ray based on where the user clicks on the screen
        Ray create_click_ray(int mouseX, int mouseY, int screenWidth, int screenHeight, const Scene::Camera* cam);


        ////////////////////////////////////////////////
        //                Attributes                  //
        ////////////////////////////////////////////////

        MagpieGame game;
        Scene scene;

        Scene::Camera* camera = nullptr;
        Scene::Transform* camera_trans = nullptr;

        // Used for updating animations in the scene
        std::vector< AnimatedModel* > animated_scene_objects;

        // Tracks what walls or floor tiles need to be changed
        // back to using the vertex_color_program
        std::vector< Wall* > transparent_walls;
        std::vector< FloorTile* > highlighted_tiles;

        //UI testing
        UI ui = UI(1, 1);
    };
}
