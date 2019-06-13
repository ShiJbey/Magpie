
#include "startmenu.hpp"
#include "../base/Load.hpp"
#include "../base/data_path.hpp"
#include "../base/vertex_color_program.hpp"
#include "../ui/draw_freetype_text.hpp"

Load< Scene::Object::ProgramInfo > vertex_color_program_info(LoadTagDefault, []() {
    Scene::Object::ProgramInfo* info = new Scene::Object::ProgramInfo();
    info->program = vertex_color_program->program;
    info->mvp_mat4 = vertex_color_program->object_to_clip_mat4;
    info->mv_mat4x3 = vertex_color_program->object_to_light_mat4x3;
    info->itmv_mat3 = vertex_color_program->normal_to_light_mat3;
    return info;
});

Load< MeshBuffer > title_mesh(LoadTagDefault, []() {
    return new MeshBuffer(data_path("title.pnc"));
});

Load< GLuint > title_vao(LoadTagDefault, []() {
    return new GLuint(title_mesh->make_vao_for_program(vertex_color_program->program));
});

Load< MeshBuffer > win_mesh(LoadTagDefault, []() {
    return new MeshBuffer(data_path("win.pnc"));
});

Load< GLuint > win_vao(LoadTagDefault, []() {
    return new GLuint(win_mesh->make_vao_for_program(vertex_color_program->program));
});

Load< MeshBuffer > lose_mesh(LoadTagDefault, []() {
    return new MeshBuffer(data_path("lose.pnc"));
});

Load< GLuint > lose_vao(LoadTagDefault, []() {
    return new GLuint(lose_mesh->make_vao_for_program(vertex_color_program->program));
});

Scene::Camera *title_camera;
Load< Scene > title_scene(LoadTagDefault, [](){
    Scene *ret = new Scene;
    //load transform hierarchy:
    ret->load(data_path("title.scene"), [](Scene &s, Scene::Transform *t, std::string const &m){
        Scene::Object *obj = s.new_object(t);

        obj->programs[Scene::Object::ProgramTypeDefault] = *vertex_color_program_info.value;
        obj->programs[Scene::Object::ProgramTypeDefault].vao = *title_vao;

        MeshBuffer::Mesh const &mesh = title_mesh->lookup(m);
        obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
        obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
    });

    //look up the camera:
    for (Scene::Camera *c = ret->first_camera; c != nullptr; c = c->alloc_next) {
        if (c->transform->name == "title_cam") {
            if (title_camera) throw std::runtime_error("Multiple 'title cam' objects in scene.");
            title_camera = c;
        }
    }
    if (!title_camera) throw std::runtime_error("No 'title cam' camera in scene.");
    return ret;
});

Scene::Camera *win_camera;
Load< Scene > win_scene(LoadTagDefault, [](){
    Scene *ret = new Scene;
    //load transform hierarchy:
    ret->load(data_path("win.scene"), [](Scene &s, Scene::Transform *t, std::string const &m){
        Scene::Object *obj = s.new_object(t);

        obj->programs[Scene::Object::ProgramTypeDefault] = *vertex_color_program_info.value;
        obj->programs[Scene::Object::ProgramTypeDefault].vao = *win_vao;


        MeshBuffer::Mesh const &mesh = win_mesh->lookup(m);
        obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
        obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
    });

    //look up the camera:
    for (Scene::Camera *c = ret->first_camera; c != nullptr; c = c->alloc_next) {
        if (c->transform->name == "win_cam") {
            if (win_camera) throw std::runtime_error("Multiple 'win cam' objects in scene.");
            win_camera = c;
        }
    }
    if (!win_camera) throw std::runtime_error("No 'win cam' camera in scene.");
    return ret;
});

Scene::Camera *lose_camera;
Load< Scene > lose_scene(LoadTagDefault, [](){
    Scene *ret = new Scene;
    //load transform hierarchy:
    ret->load(data_path("lose.scene"), [](Scene &s, Scene::Transform *t, std::string const &m){
        Scene::Object *obj = s.new_object(t);

        obj->programs[Scene::Object::ProgramTypeDefault] = *vertex_color_program_info.value;
        obj->programs[Scene::Object::ProgramTypeDefault].vao = *lose_vao;

        MeshBuffer::Mesh const &mesh = lose_mesh->lookup(m);
        obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
        obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
    });

    //look up the camera:
    for (Scene::Camera *c = ret->first_camera; c != nullptr; c = c->alloc_next) {
        if (c->transform->name == "lose_cam") {
            if (lose_camera) throw std::runtime_error("Multiple 'lose cam' objects in scene.");
            lose_camera = c;
        }
    }
    if (!lose_camera) throw std::runtime_error("No 'lose cam' camera in scene.");
    return ret;
});

void Magpie::StartMenu::draw(glm::uvec2 const &drawable_size) {
    glViewport(0, 0, drawable_size.x, drawable_size.y);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //set up basic OpenGL state:
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    { // Draw the scene
        //set up light positions:
        glUseProgram(vertex_color_program->program);

        //don't use distant directional light at all (color == 0):
        glUniform3fv(vertex_color_program->sun_color_vec3, 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
        glUniform3fv(vertex_color_program->sun_direction_vec3, 1, glm::value_ptr(glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f))));
        //use hemisphere light for sky light:
        glUniform3fv(vertex_color_program->sky_color_vec3, 1, glm::value_ptr(glm::vec3(0.65f, 0.6f, 0.7f)));
        glUniform3fv(vertex_color_program->sky_direction_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.2f, 1.0f)));

        title_camera->aspect = drawable_size.x / float(drawable_size.y);
        //Draw scene:
        title_scene->draw(title_camera);
    }

    MenuMode::draw(drawable_size);
}

void Magpie::EndMenu::draw(glm::uvec2 const &drawable_size) {
    glViewport(0, 0, drawable_size.x, drawable_size.y);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //set up basic OpenGL state:
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    { // Draw the scene
        //set up light positions:
        glUseProgram(vertex_color_program->program);

        //don't use distant directional light at all (color == 0):
        glUniform3fv(vertex_color_program->sun_color_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glUniform3fv(vertex_color_program->sun_direction_vec3, 1, glm::value_ptr(glm::normalize(glm::vec3(-1.0f, -1.0f, 1.0f))));
        //use hemisphere light for sky light:
        glUniform3fv(vertex_color_program->sky_color_vec3, 1, glm::value_ptr(glm::vec3(0.9f, 0.9f, 0.9f)));
        glUniform3fv(vertex_color_program->sky_direction_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));

        if (game_win) {
            win_camera->aspect = drawable_size.x / float(drawable_size.y);
            win_scene->draw(lose_camera);
            RenderText(ransom_font.value, "You stole $" + std::to_string(game_score) + "!",
                    0.f, 0.f, 1.f, glm::vec3(1.f, 1.f, 1.f));
        } else {
            lose_camera->aspect = drawable_size.x / float(drawable_size.y);
            lose_scene->draw(lose_camera);
            RenderText(ransom_font.value, "game over",
                       0.f, 0.f, 1.f, glm::vec3(1.f, 1.f, 1.f));
        }
    }

    MenuMode::draw(drawable_size);
}