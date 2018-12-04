#pragma once

#include "MenuMode.hpp"
#include "MagpieGamemode.hpp"

#include <functional>
#include <vector>
#include <string>

namespace Magpie {
    struct StartMenu : public MenuMode {
        StartMenu() {
            choices.emplace_back("MAGPIE");
            choices.emplace_back("PLAY", [](){
                Mode::set_current(std::make_shared< Magpie::MagpieGameMode >());
            });
            choices.emplace_back("QUIT", [](){
                Mode::set_current(nullptr);
            });

            selected = 1;
        }
    };

    struct EndMenu : public MenuMode {
        EndMenu() {
            choices.emplace_back("GAME OVER");
            selected = 1;
        }
    };
}
