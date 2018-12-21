#include "AnimatedText.hpp"

///////////////////////////////////////////////
//             ANIMATED TEXT                 //
/////////////////////////////////////////////// 

Magpie::AnimatedText::AnimatedText(std::string text, const std::map< GLchar, Character >* font, glm::vec2 anchor, float scale,  glm::vec4 color_rgba, float animation_duration) {
    this->text = text;
    this->font = font;
    this->anchor = anchor;
    this->scale = scale;
    this->color = color_rgba;
    this->animation_duration = animation_duration;
    this->elapsed_animation_time = 0.0f;
};

void Magpie::AnimatedText::update_animation(float elapsed) {
    elapsed_animation_time += elapsed;
};

void Magpie::AnimatedText::draw() {
    color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    RenderText(font, text, anchor.x, anchor.y, scale, color);
};

bool Magpie::AnimatedText::animation_complete() {
    return elapsed_animation_time >= animation_duration;
};

///////////////////////////////////////////////
//        FLOATING NOTIFICATION TEXT         //
/////////////////////////////////////////////// 

Magpie::FloatingNotificationText::FloatingNotificationText(std::string text, const std::map< GLchar, Character >* font, glm::vec2 anchor, float scale,  glm::vec4 color_rgba, float animation_duration)
    : AnimatedText(text, font, anchor, scale, color_rgba, animation_duration) {
    this->original_scale = scale;
};

Magpie::FloatingNotificationText::~FloatingNotificationText() {};

void Magpie::FloatingNotificationText::update_animation(float elapsed) {
    elapsed_animation_time += elapsed;

    if (elapsed_animation_time < (animation_duration / 2.0f)) {
        this->scale = original_scale * (elapsed_animation_time / (animation_duration / 2.0f));
    } else {
        this->scale = original_scale;
    }

    // Floating animations start of at half the scale and get larger
    // until it reaches the original scale.
    

    // To have the text float upwards, increment the x anchor slightly
    // at 10 pixels per second
    this->anchor.y += 30.0f * elapsed;
};

///////////////////////////////////////////////
//              WAITING TEXT                 //
/////////////////////////////////////////////// 

Magpie::WaitingText::WaitingText(std::string text, const std::map< GLchar, Character >* font, glm::vec2 anchor, float scale,  glm::vec4 color_rgba, float animation_duration)
    : AnimatedText(text, font, anchor, scale, color_rgba, animation_duration) {
    this->periods_added = 0;
    this->original_text = text;
};

void Magpie::WaitingText::update_animation(float elapsed) {
    elapsed_animation_time += elapsed;
    
    if (elapsed >= animation_duration / 4.0f && periods_added < 1) {
        text = original_text + ".";
        periods_added = 1;
    }
    else if (elapsed >= animation_duration / 2.0f && periods_added < 2) {
        text = original_text + "..";
        periods_added = 2;
    }
    else if (elapsed >= (3.0f * animation_duration / 4.0f) && periods_added < 3) {
        text = original_text + "...";
        periods_added = 2;
    }
};