#pragma once

#include "Player.hpp"
#include "Guard.hpp"
#include "MagpieLevel.hpp"
#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <deque>


namespace Magpie {

    // Using this class to organize all important game logic
    class MagpieGame {
    public:

        Player* get_player();
        std::vector< Guard* > get_guards();
        MagpieLevel* get_level();

        void add_guard(Guard* guard);
        void set_player(Player* player);
        void set_guards(std::vector< Guard* > guard_vec);
        void set_level(MagpieLevel* level);
        void trigger_escape();
        float escape_update(float elapsed);  //returns time remaining in counter

        std::shared_ptr< Sound::PlayingSample >current_music;

        const float escape_timer = 47.f;
        float elapsed_in_escape = -1.f;
        bool escape_started = false;

    protected:
        // Characters within the game
        Player* player;
        std::vector< Guard* > guards;

        // Level being displayed to the player
        MagpieLevel* current_level;
    };

}