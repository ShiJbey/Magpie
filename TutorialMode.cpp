#include "TutorialMode.hpp"

#include "Load.hpp"
#include "compile_program.hpp"
#include "draw_text.hpp"
#include "draw_freetype_text.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>

//---------- resources ------------

GLint tutorial_fade_program_color = -1;

Load< GLuint > tutorial_fade_program(LoadTagInit, [](){
	GLuint *ret = new GLuint(compile_program(
		"#version 330\n"
		"void main() {\n"
		"	gl_Position = vec4(4 * (gl_VertexID & 1) - 1,  2 * (gl_VertexID & 2) - 1, 0.0, 1.0);\n"
		"}\n"
	,
		"#version 330\n"
		"uniform vec4 color;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"	fragColor = color;\n"
		"}\n"
	));

	tutorial_fade_program_color = glGetUniformLocation(*ret, "color");

	return ret;
});

//----------------------

bool TutorialMode::handle_event(SDL_Event const &e, glm::uvec2 const &window_size) {
	if (e.type == SDL_KEYDOWN) {
		if (e.key.keysym.sym == SDLK_ESCAPE) {
			if (on_escape) {
				on_escape();
			} else {
				Mode::set_current(background);
			}
			return true;
		}
	}
	return false;
}

void TutorialMode::update(float elapsed) {
	if (background) {
		background->update(elapsed * background_time_scale);
	}
}

void TutorialMode::draw(glm::uvec2 const &drawable_size) {
	if (background && background_fade < 1.0f) {
		background->draw(drawable_size);

		glDisable(GL_DEPTH_TEST);
		if (background_fade > 0.0f) {
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUseProgram(*tutorial_fade_program);
			glUniform4fv(tutorial_fade_program_color, 1, glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, background_fade)));
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glUseProgram(0);
			glDisable(GL_BLEND);
		}
	}
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);



    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float aspect = viewport[2] / float(viewport[3]);
    float x_margin = 0.2f;
    float y_margin = 0.17f;
    float tab = 0.7f;
    float section_break = 0.08f;

	float font_height = 0.05f;
	float padding = 0.1f;
    glm::vec2 start = glm::vec2(-aspect+x_margin, 1-y_margin);
    glm::vec2 anchor = start;

    auto write_line = [&](std::string text) {
        float target_font_size = viewport[3] * font_height;
        float font_scale = target_font_size / 64.f;
		anchor.y -= font_height;

		glm::vec2 window_anchor = glm::vec2();
		window_anchor.x = ((anchor.x + aspect) / (2*aspect)) * viewport[2];
		window_anchor.y = ((anchor.y + 1) / 2) * viewport[3];

		RenderText(tutorial_font.value, text, window_anchor.x, window_anchor.y, font_scale, glm::vec4(0.f,1.f,0.f,1.f));
		anchor.y -= padding;
    };

    font_height = 0.06f;
    padding = 0.17f;
	write_line("EARN MONEY BY STEALING ");
	anchor.y -= section_break;

	write_line("PICK UP KEY ITEMS TO UNLOCK NEW SKILLS");
	anchor.x += tab;
	font_height = 0.04f;
	padding = 0.2f;
	write_line("KEYCARDS unlock doors");
	write_line("DOG TREATS distract guards");
	write_line("KEY unlock display cases without breaking them");
	write_line("BOXES disguise you from guards");
    anchor.y -= section_break;

	anchor.x = start.x;
	font_height = 0.06f;
	padding = 0.17f;
	write_line("ESCAPE WITHOUT GETTING CAUGHT");

	glEnable(GL_DEPTH_TEST);
}
