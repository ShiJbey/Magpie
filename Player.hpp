#pragma once

#include "KeyFrameAnimation.hpp"

enum PlayerState {
    IDLE,
    WALKING,
    STEALING
};

struct Player {
    PlayerState state = IDLE;

    float animation_time = 0.0f;

    uint32_t keyframe_index;

    Magpie::KeyFrameAnimation anim;
};