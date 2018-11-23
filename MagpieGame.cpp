#include "MagpieGame.hpp"

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