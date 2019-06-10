#pragma once

#include "characters/Player.hpp"
#include "characters/Guard.hpp"
#include "level/MagpieLevel.hpp"
#include "base/Scene.hpp"
#include "base/Sound.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <deque>


namespace Magpie {

    // Using this class to maintain the games's state
    class MagpieGame {
    public:
        MagpieGame() { this->game_over = false; }

        // Getters
        Player* get_player();
        std::vector< Guard* > get_guards();
        MagpieLevel* get_level();
        bool is_game_over() { return this->game_over; }
        bool has_escape_started() { return this->escape_started; }
        float get_elapsed_in_escape() { return this->elapsed_in_escape; }
        std::shared_ptr< Sound::PlayingSample > get_current_music() {
            return this->current_music;
        }
        bool has_player_won() { return this->player_won; }

        // Setters
        void set_player(Player* player);
        void set_guards(std::vector< Guard* > guard_vec);
        void set_level(MagpieLevel* level);
        void set_game_over(bool game_over) { this->game_over = game_over; }
        void set_elapsed_in_escape(float time) { this->elapsed_in_escape = time; }
        void set_escape_started(bool is_started) { this->escape_started = is_started; }
        void set_current_music(std::shared_ptr< Sound::PlayingSample > sample) {
            this->current_music = sample;
        }
        void set_player_won(bool won) { this->player_won = won; }

        // Adds a single guard to the level
        void add_guard(Guard* guard);

        // Starts the escape sequence
        void trigger_escape();

        /**
         * Special update for when the escape sequence
         * starts. It returns the time remaining in the counter
         */
        void escape_update(float elapsed);


        // Escape sequence
        const float ESCAPE_TIME = 60.f;

    protected:
        // Characters
        Player* player;
        std::vector< Guard* > guards;

        // Level being displayed
        MagpieLevel* current_level;

        // Track if the game has ended
        bool game_over;
        bool player_won;

        // Escape sequence
        float elapsed_in_escape = -1.f;
        bool escape_started = false;

        // Bakground game music
        std::shared_ptr< Sound::PlayingSample > current_music;
    };

}