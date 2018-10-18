#include "MagpieGamemode.hpp"

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
#include "texture_program.hpp"
#include "vertex_color_program.hpp"
#include "depth_program.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <map>
#include <cstddef>
#include <random>
#include <unordered_map>

namespace Magpie {

    Scene::Transform *camera_parent_transform = nullptr;
    Scene::Camera *camera = nullptr;
    Scene::Transform *spot_parent_transform = nullptr;
    Scene::Lamp *spot = nullptr;
    std::vector< Scene::Transform* > player_model_walk_transforms;
    std::vector< Scene::Transform* > player_model_steal_transforms;
    std::vector< Scene::Transform* > player_model_idle_transforms;
    std::list< TransformAnimationPlayer > current_animations;
    Scene::Transform *player_trans = nullptr;

    // PLAYER IDLE
    Load< MeshBuffer > magpie_player_idle_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie_idle.pnc"));
    });

    Load< GLuint > magpie_player_idle_mesh_for_vertex_color_program(LoadTagDefault, [](){
        return new GLuint(magpie_player_idle_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > player_walk_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("player_walk.tanim"));
    });



    Load< MeshBuffer > magpie_player_walk_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie_walk.pnc"));
    });

    Load< GLuint > magpie_player_walk_mesh_for_vertex_color_program(LoadTagDefault, [](){
        return new GLuint(magpie_player_walk_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< MeshBuffer > magpie_player_steal_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie_steal.pnc"));
    });

    Load< GLuint > magpie_player_steal_mesh_for_vertex_color_program(LoadTagDefault, [](){
        return new GLuint(magpie_player_steal_mesh->make_vao_for_program(vertex_color_program->program));
    });

    // PLAYER ANIMS
    

    Load< TransformAnimation > player_idle_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("player_idle.tanim"));
    });

    Load< TransformAnimation > player_steal_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("player_steal.tanim"));
    });

    // ENV

    Load< MeshBuffer > magpie_meshes(LoadTagDefault, [](){
        return new MeshBuffer(data_path("prototype_scene.pnct"));
    });

    Load< GLuint > meshes_for_vertex_color_program(LoadTagDefault, [](){
        return new GLuint(magpie_meshes->make_vao_for_program(vertex_color_program->program));
    });

    // GUARD
    
    Load< MeshBuffer > magpie_guard_patrol_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("guardDog_patrol.pnc"));
    });

    Load< GLuint > magpie_guard_patrol_mesh_for_vertex_color_program(LoadTagDefault, [](){
        return new GLuint(magpie_guard_patrol_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > guard_patrol_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("guardDog_patrol.tanim"));
    });


    Load< Scene > scene(LoadTagDefault, [](){
        Scene *ret = new Scene;

        Scene::Object::ProgramInfo scene_vertex_color_program_info;
        scene_vertex_color_program_info.program = vertex_color_program->program;
        scene_vertex_color_program_info.vao = *meshes_for_vertex_color_program;
        scene_vertex_color_program_info.mvp_mat4 = vertex_color_program->object_to_clip_mat4;
        scene_vertex_color_program_info.mv_mat4x3 = vertex_color_program->object_to_light_mat4x3;
        scene_vertex_color_program_info.itmv_mat3 = vertex_color_program->normal_to_light_mat3;

        Scene::Object::ProgramInfo walk_vertex_color_program_info;
        walk_vertex_color_program_info.program = vertex_color_program->program;
        walk_vertex_color_program_info.vao = *magpie_player_idle_mesh_for_vertex_color_program;
        walk_vertex_color_program_info.mvp_mat4 = vertex_color_program->object_to_clip_mat4;
        walk_vertex_color_program_info.mv_mat4x3 = vertex_color_program->object_to_light_mat4x3;
        walk_vertex_color_program_info.itmv_mat3 = vertex_color_program->normal_to_light_mat3;
        
        Scene::Object::ProgramInfo patrol_vertex_color_program_info;
        walk_vertex_color_program_info.program = vertex_color_program->program;
        walk_vertex_color_program_info.vao = *magpie_guard_patrol_mesh_for_vertex_color_program;
        walk_vertex_color_program_info.mvp_mat4 = vertex_color_program->object_to_clip_mat4;
        walk_vertex_color_program_info.mv_mat4x3 = vertex_color_program->object_to_light_mat4x3;
        walk_vertex_color_program_info.itmv_mat3 = vertex_color_program->normal_to_light_mat3;
        

        ret->load(data_path("magpie_idle.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            obj->programs[Scene::Object::ProgramTypeDefault] = walk_vertex_color_program_info;

            MeshBuffer::Mesh const &mesh = magpie_player_idle_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });

        player_trans = ret->look_up("magpieIdle_GRP");
        assert(player_trans != nullptr);
        
        //load transform hierarchy:
        ret->load(data_path("prototype_scene.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            
            obj->programs[Scene::Object::ProgramTypeDefault] = scene_vertex_color_program_info;

            MeshBuffer::Mesh const &mesh = magpie_meshes->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });

        
        ret->load(data_path("guardDog_patrol.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            obj->programs[Scene::Object::ProgramTypeDefault] = patrol_vertex_color_program_info;

            MeshBuffer::Mesh const &mesh = magpie_guard_patrol_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });

        //look up various transforms for animations
        std::unordered_map< std::string, Scene::Transform * > name_to_transform;
        for (Scene::Transform *t = ret->first_transform; t != nullptr; t = t->alloc_next) {
            auto ret = name_to_transform.insert(std::make_pair(t->name, t));
            if (!ret.second) {
                std::cerr << "WARNING: multiple transforms with the name '" << t->name << "' in scene." << std::endl;
            }
        }

        for (auto const &name : player_walk_tanim->names) {
            auto f = name_to_transform.find(name);
            if (f == name_to_transform.end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                player_model_walk_transforms.emplace_back(nullptr);
            } else {
                player_model_walk_transforms.emplace_back(f->second);
            }
        }

        for (auto const &name : player_idle_tanim->names) {
            auto f = name_to_transform.find(name);
            if (f == name_to_transform.end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                player_model_idle_transforms.emplace_back(nullptr);
            } else {
                player_model_idle_transforms.emplace_back(f->second);
            }
        }
        
        // Start walk animation
        current_animations.emplace_back(*player_walk_tanim, player_model_walk_transforms, 1.0f, true);
        current_animations.emplace_back(*player_idle_tanim, player_model_idle_transforms, 1.0f, true);

        //look up the camera:
        for (Scene::Camera *c = ret->first_camera; c != nullptr; c = c->alloc_next) {
            if (c->transform->name == "Camera") {
                if (camera) throw std::runtime_error("Multiple 'Camera' objects in scene.");
                camera = c;
            }
        }
        if (!camera) throw std::runtime_error("No 'Camera' camera in scene.");

        //look up the spotlight:
        for (Scene::Lamp *l = ret->first_lamp; l != nullptr; l = l->alloc_next) {
            if (l->transform->name == "Spot") {
                if (spot) throw std::runtime_error("Multiple 'Spot' objects in scene.");
                if (l->type != Scene::Lamp::Spot) throw std::runtime_error("Lamp 'Spot' is not a spotlight.");
                spot = l;
            }
        }
        if (!spot) throw std::runtime_error("No 'Spot' spotlight in scene.");

        return ret;
    });

    MagpieGameMode::MagpieGameMode() {
        Grid currFloor = Grid();
        currFloor.initGrid("prototype");
    };

    MagpieGameMode::~MagpieGameMode() {

    };

    void MagpieGameMode::updatePosition(char character, std::vector<glm::uvec2> path) {
        for (uint32_t i=0; i<path.size(); i++) {
            if (character == 'm' && magMoveCountdown<=0.0f) { //magpie
                magpie = path[i]; //set new position
                //MAYBE WALKING ANIMATION HERE
                magMoveCountdown = 5.0f;
            }
        }
    };

    void MagpieGameMode::update(float elapsed) {
        magMoveCountdown -= elapsed;

        glm::vec2 vel(((float)magpieEndpt.x - player_trans->position.x), ((float)magpieEndpt.y - player_trans->position.y));
        //vel = vel / glm::length(vel);
        //vel *= 5.0;
        player_trans->position.x += elapsed * vel.x;
        player_trans->position.y += elapsed * vel.y;

        //player_trans->position.z += 0.1f;

        for (auto ca = current_animations.begin(); ca != current_animations.end(); /* later */ ) {
            ca->update(elapsed);
            if (ca->done()) {
                auto old = ca;
                ++ca;
                current_animations.erase(old);
            } else {
                ++ca;
            }
        }
    };

    glm::uvec2 MagpieGameMode::tileMap(glm::vec3 isect, std::string floorPlan) {
        float r = std::floor(isect.x);
        float c = std::floor(isect.y);
        bool negative = (r<0.0f || c<0.0f);
        bool outOfRange = (r>=currFloor.rows || c>=currFloor.cols);
        if (negative || outOfRange) {
            //click is negative and impossible or is greater than dims of row and cols of given map
            return glm::uvec2(-1, -1);
        }
        return glm::uvec2(r, c);
    };

    //from this tutorial: http://antongerdelan.net/opengl/raycasting.html
    glm::uvec2 MagpieGameMode::mousePick(int mouseX, int mouseY, int screenWidth, int screenHeight,
                                int floorHeight, const Scene::Camera* cam, std::string flPlan) {
        //cam from scene::
        //const Scene::Camera* cam = ;
        glm::mat4 camLocalToWorld = cam->transform->make_local_to_world(); 

        float halfImageHeight = cam->near*std::tan(cam->fovy/2.0f);
        float halfImageWidth = cam->aspect*halfImageHeight;

        //3d Normalized device coords
        float normDeviceX = (2.0f * mouseX) / screenWidth - 1.0f;
        float normDeviceY = 1.0f - (2.0f * mouseY) / screenHeight;
        glm::vec3 localOrigin = glm::vec3(normDeviceX*halfImageWidth, normDeviceY*halfImageHeight, -cam->near);
        glm::vec3 worldOrigin = camLocalToWorld*glm::vec4(localOrigin, 1.0f);
        glm::vec3 worldDir = camLocalToWorld*glm::vec4(localOrigin, 0.0f); //unnormalized dir

        //float dist = worldOrigin.z - floorHeight;

        if (worldDir.z>=0.0f) { //discard all rays going away from floor
            return glm::uvec2(-1,-1);
        }
        //want to solve s.t. floorHeight = origin.z + t*worldDir.z
        float t = (floorHeight-worldOrigin.z)/worldDir.z;

        glm::vec3 pointOfIntersect = worldOrigin + t*worldDir;

        glm::uvec2 pickedTile = tileMap(pointOfIntersect, flPlan);

        return pickedTile;
    };

    bool MagpieGameMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
        //ignore any keys that are the result of automatic key repeat:
        if (evt.type == SDL_KEYDOWN && evt.key.repeat) {
            return false;
        }

        if (evt.type == SDL_MOUSEBUTTONDOWN) {
            if (evt.button.button == SDL_BUTTON_LEFT) {
                //TODO: get x and y of mouse click and figure out which tile it is
                glm::uvec2 clickedTile = mousePick(evt.button.x, evt.button.y, 
                                        window_size.x, window_size.y, 0, camera, "prototype");
                std::cout << "clickedTile.x is "<< clickedTile.x << "and clickTile.y is "<< clickedTile.y << std::endl;
                if (clickedTile.x>=0 && clickedTile.y>=0) { //ignore (-1, -1)/error
                    //pass into pathfinding algorithm
                    ;
                    magpieEndpt = clickedTile;
                    //player_trans->position.x = (float)clickedTile.x;
                    //player_trans->position.y = (float)clickedTile.y;
                }
            }
        }

        return false;
    };

    void MagpieGameMode::draw(glm::uvec2 const &drawable_size) {
        glViewport(0,0,drawable_size.x, drawable_size.y);
        
    

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
            glUniform3fv(vertex_color_program->sun_direction_vec3, 1, glm::value_ptr(glm::normalize(glm::vec3(0.0f, 0.0f,-1.0f))));
            //use hemisphere light for sky light:
            glUniform3fv(vertex_color_program->sky_color_vec3, 1, glm::value_ptr(glm::vec3(0.9f, 0.9f, 0.95f)));
            glUniform3fv(vertex_color_program->sky_direction_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
            camera->aspect = drawable_size.x / float(drawable_size.y);
            //Draw scene:
            scene->draw(camera);
        }

        GL_ERRORS();
    };



}