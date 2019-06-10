#include "MagpieGame.hpp"
#include "AssetLoader.hpp"
#include "game_settings.hpp"

Magpie::Player* Magpie::MagpieGame::get_player() {
    return this->player;
};

std::vector< Magpie::Guard* > Magpie::MagpieGame::get_guards() {
    return this->guards;
};

Magpie::MagpieLevel* Magpie::MagpieGame::get_level() {
    return this->current_level;
};

void Magpie::MagpieGame::add_guard(Magpie::Guard* guard) {
    this->guards.push_back(guard);
};

void Magpie::MagpieGame::set_player(Magpie::Player* player) {
    this->player = player;
};

void Magpie::MagpieGame::set_guards(std::vector< Magpie::Guard* > guard_vec) {
    this->guards = guard_vec;
};

void Magpie::MagpieGame::set_level(Magpie::MagpieLevel* level) {
    this->current_level = level;
};

void Magpie::MagpieGame::trigger_escape() {
    if (this->elapsed_in_escape >= 0) return;
    this->elapsed_in_escape = 0;
    this->current_music->stop(1.0f);
};

void Magpie::MagpieGame::escape_update(float elapsed) {
    if (this->elapsed_in_escape >= 0) {

        //start escape stage with delay to let previous UI/sounds fade
        if (this->elapsed_in_escape >= 3.0f && !this->escape_started) {
            set_escape_started(true);
            if (!mute_sound) {
                sample_siren->play(this->get_player()->get_position());
                this->current_music = sample_ambient_faster->play(this->get_player()->get_position(), 0.4f, Sound::Loop);
            }

        }

        this->elapsed_in_escape += elapsed;

        if (this->ESCAPE_TIME - this->elapsed_in_escape <= 0) {
            set_game_over(true);
        };
    }
};