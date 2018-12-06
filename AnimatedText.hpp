#pragma once

#include "draw_freetype_text.hpp"
#include <glm/glm.hpp>

namespace Magpie {

    struct AnimatedText {

        // Properties for drawing
        std::string text;
        const std::map< GLchar, Character >* font;
        glm::vec2 anchor;
        float scale;
        glm::vec4 color;
        // Animation specific
        float animation_duration;
        float elapsed_animation_time;

        AnimatedText(std::string text, const std::map< GLchar, Character >* font, glm::vec2 anchor, float scale,  glm::vec4 color_rgba, float animation_duration);

        virtual void update_animation(float elapsed);
        virtual void draw();
        virtual bool animation_complete();

    };

    struct FloatingNotificationText : public AnimatedText {

        float original_scale;

        FloatingNotificationText(std::string text, const std::map< GLchar, Character >* font, glm::vec2 anchor, float scale,  glm::vec4 color_rgba, float animation_duration);

        virtual void update_animation(float elapsed) override;

    };

    struct WaitingText : public AnimatedText {

        std::string original_text;
        uint32_t periods_added;

        WaitingText(std::string text, const std::map< GLchar, Character >* font, glm::vec2 anchor, float scale,  glm::vec4 color_rgba, float animation_duration);

        virtual void update_animation(float elapsed) override;

    };

}