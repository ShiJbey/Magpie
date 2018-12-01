
//#include "SignalQueue.h"
#include "UI.h"
#include <iostream>

#include "Load.hpp"
#include "MeshBuffer.hpp"
#include "check_fb.hpp" //helper for checking currently bound OpenGL framebuffer
#include "read_chunk.hpp" //helper for reading a vector of structures from a file
#include "data_path.hpp" //helper to get paths relative to executable
#include "compile_program.hpp" //helper to compile opengl shader programs
#include "load_save_png.hpp"
#include "texture_program.hpp"
#include "depth_program.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <map>
#include <cstddef>
#include <random>

//TODO: MAKE A UI.PNCT
Load< MeshBuffer > ui_meshes(LoadTagDefault, [](){
    return new MeshBuffer(data_path("map.pnct"));
});

Load< GLuint > ui_meshes_for_texture_program(LoadTagDefault, [](){
    return new GLuint(ui_meshes->make_vao_for_program(texture_program->program));
});

Load< GLuint > ui_meshes_for_depth_program(LoadTagDefault, [](){
    return new GLuint(ui_meshes->make_vao_for_program(depth_program->program));
});

//used for fullscreen passes:
Load< GLuint > empt_vao(LoadTagDefault, [](){
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindVertexArray(0);
    return new GLuint(vao);
});

GLuint load_tex(std::string const &filename) {
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

Load< GLuint > wood_tex(LoadTagDefault, [](){
    return new GLuint(load_tex(data_path("textures/wood.png")));
});

Scene::Camera *camera = NULL;

Load< Scene > ui(LoadTagDefault, [](){
    Scene *ret = new Scene;

    //pre-build some program info (material) blocks to assign to each object:
    Scene::Object::ProgramInfo texture_program_info;
    texture_program_info.program = texture_program->program;
    texture_program_info.vao = *ui_meshes_for_texture_program;
    texture_program_info.mvp_mat4  = texture_program->object_to_clip_mat4;
    texture_program_info.mv_mat4x3 = texture_program->object_to_light_mat4x3;
    texture_program_info.itmv_mat3 = texture_program->normal_to_light_mat3;

    Scene::Object::ProgramInfo depth_program_info;
    depth_program_info.program = depth_program->program;
    depth_program_info.vao = *ui_meshes_for_depth_program;
    depth_program_info.mvp_mat4  = depth_program->object_to_clip_mat4;

    //TODO: MAKE MAP.SCENE
    //load transform hierarchy:
    ret->load(data_path("map.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
        Scene::Object *obj = s.new_object(t);

        obj->programs[Scene::Object::ProgramTypeDefault] = texture_program_info;
        if (t->name == "map") {
            obj->programs[Scene::Object::ProgramTypeDefault].textures[0] = *wood_tex;
        }
        //obj->programs[Scene::Object::ProgramTypeShadow] = depth_program_info;

        MeshBuffer::Mesh const &mesh = ui_meshes->lookup(m);
        obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
        obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;

        //obj->programs[Scene::Object::ProgramTypeShadow].start = mesh.start;
        //obj->programs[Scene::Object::ProgramTypeShadow].count = mesh.count;
    });

    for (Scene::Camera *c = ret->first_camera; c != nullptr; c = c->alloc_next) {
        if (c->transform->name == "Camera") {
            if (camera) throw std::runtime_error("Multiple 'Camera' objects in scene.");
            camera = c;
        }
    }
    if (!camera) throw std::runtime_error("No 'Camera' camera in scene.");

    return ret;
});



UI::UI(int object_id, int group_id) {
    objID = object_id;
    grpID = group_id;

    //initialize map
    map = Map();

    //initialize inventory
    inventory = Inventory();
}

void UI::consume_signal() {
    //get right signals
    while (has_signal()) {
        Magpie::Signal s = read_from_signal_queue();
        if (s.get_signal_number() == 0) { //high intensity endgame triggered
            endGame = true;
            //TODO: move security bar very far off screen, its irrelevant
            ;
        }
        else if (s.get_signal_number() == 1) { //one more area of map unlocked
            if (map.unlockedAreas==1 || map.unlockedAreas==2) {map.unlockedAreas++;}
        }
        else if (s.get_signal_number() == 2 || s.get_signal_number() == 3) { //security level change
            if (s.get_signal_number() == 2) { //increase
                if (secureLvl < 3) {secureLvl++;}
            }
            else if (s.get_signal_number() == 3) { //decrease
                if (secureLvl > 0) {secureLvl--;}
            }
            //TODO: also change how security level bar looks
            if (secureLvl == 1) {
                ;
            }
            else if (secureLvl == 2) {
                ;
            }
            else if (secureLvl == 3) {
                ;
            }
        }
        else if (s.get_signal_number()>10 && s.get_signal_number()<100000) { //score increase
            //score will only ever increase by 10
            displayScore += 10;
        }
    }
}

void UI::stateChanger(char keyPressed) {
    if (keyPressed == 'i') {
        //no matter what inv is map should not be out
        map.state = Map::OFF;
        if (inventory.state==Inventory::IN || 
            inventory.state == Inventory::IDLEIN) {
            inventory.state = Inventory::OUT; //inv scrolling reverses direction
        }
        else if (inventory.state==Inventory::OUT || 
                inventory.state==Inventory::IDLEOUT) {
            inventory.state = Inventory::IN; //inv scrolling reverses direction
        }
        //std::cout<<"state of map is now: "<<map.state<<std::endl;
        std::cout<<"state of inv is now: "<<inventory.state<<std::endl;
    }
    else if (keyPressed == 'm') {
        //no matter if map is being called or dismissed inventory should not be out
        inventory.state = Inventory::IDLEIN;
        if (map.state == Map::OFF) {
            //set state of map to ON
            map.state = Map::ON;
        }
        else if (map.state == Map::ON) {
            //set state of map to OFF
            map.state = Map::OFF;
        }
        //std::cout<<"state of map is now: "<<map.state<<std::endl;
        std::cout<<"state of inv is now: "<<inventory.state<<std::endl;
    }
}

void UI::drawUI(Scene::Camera const *cam) {
    { //drawing setup
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

        camera->aspect = cam->aspect;
    }
    { //draw everything in UI that isn't map or inventory
        /*
        TODO:
        things that will be drawn by ui->draw() [security level gauge]
        things that need to be drawn by draw_text [score, endgame countdown]
        */
        ui->draw(camera);
        //score
        ;
        if (endGame == true) { //endgame countdown
            ;
        }
    }
    inventory.drawInv(camera);
    map.drawMap(camera);
}


