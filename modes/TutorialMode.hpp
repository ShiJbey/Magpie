#pragma once

#include "../base/Mode.hpp"

#include <functional>
#include <vector>
#include <string>

struct TutorialMode : public Mode {
	virtual ~TutorialMode() { }

	virtual bool handle_event(SDL_Event const &event, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//called when user presses 'escape':
	// (note: if not defined, menumode will Mode::set_current(background).)
	std::function< void() > on_escape;

	//will render this mode in the background if not null:
	std::shared_ptr< Mode > background;
	float background_time_scale = 1.0f;
	float background_fade = 0.5f;
};
