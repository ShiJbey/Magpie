#pragma once

#include "../base/MenuMode.hpp"
#include "../modes/MagpieGamemode.hpp"
#include "../modes/TutorialMode.hpp"

#include <functional>
#include <vector>
#include <string>

namespace Magpie {
    struct StartMenu : public MenuMode {
        StartMenu() {
            choices.emplace_back("PLAY", [](){
                std::shared_ptr< Magpie::MagpieGameMode > gamemode = std::make_shared< Magpie::MagpieGameMode >();
                Mode::set_current(gamemode);
                gamemode->show_tutorial();
            });
            choices.emplace_back("QUIT", [](){
                Mode::set_current(nullptr);
            });
            selected = 0;
        }

        virtual void draw(glm::uvec2 const &drawable_size) override;
    };

    struct EndMenu : public MenuMode {
        EndMenu(bool win = false, uint32_t score = 0) {
            choices.emplace_back("QUIT", [](){
                Mode::set_current(nullptr);
            });
            selected = 0;

            game_win = win;
            game_score = score;
        }

        virtual void draw(glm::uvec2 const &drawable_size) override;

        bool game_win = false;
        uint32_t game_score = 0;
    };
}
