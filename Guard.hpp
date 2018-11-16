#pragma once

#include "GameAgent.hpp"
#include "Signalable.hpp"
#include "GameCharacter.hpp"

namespace Magpie {
    class Guard: public GameCharacter, public GameAgent, public Signalable {
    public:
       enum class STATE {
            IDLE = 0,
            PATROLING,
            CHASING,
            ALERT,
            CONFUSED,
            CAUTIOUS
        }; 

        void walk(float elapsed);

        void consume_signal();

        void update(float elapsed);

        void update_state(float elapsed);

        void interact();        
    };
}