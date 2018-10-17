#include "TestAnimationMode.hpp"
#include "TransformAnimation.hpp"

#include "MenuMode.hpp"
#include "Load.hpp"
#include "MeshBuffer.hpp"
#include "Scene.hpp"
#include "gl_errors.hpp" //helper for dumpping OpenGL error messages
#include "check_fb.hpp" //helper for checking currently bound OpenGL framebuffer
#include "read_chunk.hpp" //helper for reading a vector of structures from a file
#include "data_path.hpp" //helper to get paths relative to executable
#include "compile_program.hpp" //helper to compile opengl shader programs
#include "draw_text.hpp" //helper to... um.. draw text
#include "load_save_png.hpp"
#include "texture_program.hpp"
#include "vertex_color_program.hpp"
#include "depth_program.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <map>
#include <cstddef>
#include <random>


Load< MeshBuffer > magpie_meshes(LoadTagDefault, [](){
	return new MeshBuffer(data_path("pizza.pnc"));
});

Load< GLuint > meshes_for_vertex_color_program(LoadTagDefault, [](){
	return new GLuint(magpie_meshes->make_vao_for_program(vertex_color_program->program));
});

TestAnimationMode::TestAnimationMode() {
	scene = new Scene();

    Scene::Object::ProgramInfo vertex_color_program_info;
	vertex_color_program_info.program = vertex_color_program->program;
	vertex_color_program_info.vao = *meshes_for_vertex_color_program;
	vertex_color_program_info.mvp_mat4 = vertex_color_program->object_to_clip_mat4;
	vertex_color_program_info.mv_mat4x3 = vertex_color_program->object_to_light_mat4x3;
	vertex_color_program_info.itmv_mat3 = vertex_color_program->normal_to_light_mat3;

    scene->load(data_path("pizza.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
		Scene::Object *obj = s.new_object(t);
		t->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		
		obj->programs[Scene::Object::ProgramTypeDefault] = vertex_color_program_info;

		MeshBuffer::Mesh const &mesh = magpie_meshes->lookup(m);
		obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
		obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
	});

    // Add a new Camera
    Scene::Transform* camera_transform = scene->new_transform();
    camera_transform->position = glm::vec3(0.0f, 0.0, -9.0f);
	camera_transform->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    camera = scene->new_camera(camera_transform);
};

TestAnimationMode::~TestAnimationMode() {
	// Do Nothing
};

bool TestAnimationMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN && evt.key.repeat) {
		return false;
	}

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.scancode == SDL_SCANCODE_LEFT) {
			return true;
		}

		if (evt.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
			return true;
		}

		if (evt.key.keysym.scancode == SDL_SCANCODE_UP) {
			return true;
		}

		if (evt.key.keysym.scancode == SDL_SCANCODE_DOWN) {
			return true;
		}
	}

	return false;
};


void TestAnimationMode::update(float elapsed) {

};

void TestAnimationMode::draw(glm::uvec2 const &drawable_size) {
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	{ // Draw the scene
        glUseProgram(vertex_color_program->program);
        glUniform3fv(vertex_color_program->sun_color_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glUniform3fv(vertex_color_program->sun_direction_vec3, 1, glm::value_ptr(glm::normalize(glm::vec3(-0.2f, 0.2f, 1.0f))));
        glUniform3fv(vertex_color_program->sky_color_vec3, 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.3f)));
        glUniform3fv(vertex_color_program->sky_direction_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));

        scene->draw(camera);
    }

	GL_ERRORS();
};
