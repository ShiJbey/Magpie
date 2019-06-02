
#include "Map.h"
#include "../base/Load.hpp"
#include "../base/MeshBuffer.hpp"
#include "../base/check_fb.hpp" //helper for checking currently bound OpenGL framebuffer
#include "../base/read_chunk.hpp" //helper for reading a vector of structures from a file
#include "../base/data_path.hpp" //helper to get paths relative to executable
#include "../base/compile_program.hpp" //helper to compile opengl shader programs
#include "../base/load_save_png.hpp"
#include "../base/texture_program.hpp"
#include "../base/depth_program.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <map>
#include <cstddef>
#include <random>



Load< MeshBuffer > meshes(LoadTagDefault, [](){
    return new MeshBuffer(data_path("map.pnct"));
});

Load< GLuint > meshes_for_texture_program(LoadTagDefault, [](){
    return new GLuint(meshes->make_vao_for_program(texture_program->program));
});

Load< GLuint > meshes_for_depth_program(LoadTagDefault, [](){
    return new GLuint(meshes->make_vao_for_program(depth_program->program));
});

//used for fullscreen passes:
Load< GLuint > empty_vao(LoadTagDefault, [](){
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindVertexArray(0);
    return new GLuint(vao);
});

GLuint load_texture(std::string const &filename) {
    glm::uvec2 size;
    std::vector< glm::u8vec4 > data;
    load_png(filename, &size, &data, LowerLeftOrigin);

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    //GL_ERRORS();

    return tex;
}

Load< GLuint > marble_tex(LoadTagDefault, [](){
    return new GLuint(load_texture(data_path("textures/marble.png")));
});

Load< Scene > scene(LoadTagDefault, [](){
    Scene *ret = new Scene;

    //pre-build some program info (material) blocks to assign to each object:
    Scene::Object::ProgramInfo texture_program_info;
    texture_program_info.program = texture_program->program;
    texture_program_info.vao = *meshes_for_texture_program;
    texture_program_info.mvp_mat4  = texture_program->object_to_clip_mat4;
    texture_program_info.mv_mat4x3 = texture_program->object_to_light_mat4x3;
    texture_program_info.itmv_mat3 = texture_program->normal_to_light_mat3;

    Scene::Object::ProgramInfo depth_program_info;
    depth_program_info.program = depth_program->program;
    depth_program_info.vao = *meshes_for_depth_program;
    depth_program_info.mvp_mat4  = depth_program->object_to_clip_mat4;

    //load transform hierarchy:
    ret->load(data_path("map.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
        Scene::Object *obj = s.new_object(t);

        obj->programs[Scene::Object::ProgramTypeDefault] = texture_program_info;
        if (t->name == "map") {
            obj->programs[Scene::Object::ProgramTypeDefault].textures[0] = *marble_tex;
        }
        //obj->programs[Scene::Object::ProgramTypeShadow] = depth_program_info;

        MeshBuffer::Mesh const &mesh = meshes->lookup(m);
        obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
        obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;

        //obj->programs[Scene::Object::ProgramTypeShadow].start = mesh.start;
        //obj->programs[Scene::Object::ProgramTypeShadow].count = mesh.count;
    });

    return ret;
});


Map::Map(){

}

Map::~Map(){

}

void Map::drawMap(Scene::Camera const *cam) {
    if (state == ON) { //only draw map if STATE == ON
        //light the map
        //set up light positions:
        glUseProgram(texture_program->program);
        //don't use distant directional light at all (color == 0):
        glUniform3fv(texture_program->sun_color_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glUniform3fv(texture_program->sun_direction_vec3, 1, glm::value_ptr(glm::normalize(glm::vec3(0.0f, 0.0f,-1.0f))));
        //use hemisphere light for sky light:
        glUniform3fv(texture_program->sky_color_vec3, 1, glm::value_ptr(glm::vec3(0.9f, 0.9f, 0.95f)));
        glUniform3fv(texture_program->sky_direction_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));

        //avoid clipping through things
        glClear(GL_DEPTH_BUFFER_BIT);
        scene->draw(cam);
    }
}
