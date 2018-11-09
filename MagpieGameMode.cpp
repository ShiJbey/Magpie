#include "MagpieGamemode.hpp"

#include "TransformAnimation.hpp"
#include "load_level.hpp"

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

#include "PlayerAgent.h"
#include "PlayerModel.h"

#include "GuardAgent.h"
#include "GuardModel.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <map>
#include <cstddef>
#include <random>
#include <unordered_map>
#include <cstdlib>

#define ENTITY_FACTORY(name) \
[](int object_id, int group_id, Scene::Transform* transform){ \
 return new Entity(new name##Model(transform), new name##Agent(object_id, group_id));\
}\

namespace Magpie {

    Scene::Transform *camera_parent_transform = nullptr;
    Scene::Camera *camera = nullptr;
    Scene::Transform *camera_trans = nullptr;
    Scene::Transform *spot_parent_transform = nullptr;
    Scene::Lamp *spot = nullptr;

    // Tansforms for player animations
    std::vector< Scene::Transform* > player_model_walk_transforms;
    std::vector< Scene::Transform* > player_model_steal_transforms;
    std::vector< Scene::Transform* > player_model_idle_transforms;

    // Player Animation players
    TransformAnimationPlayer* current_player_animation = nullptr;
    TransformAnimationPlayer* magpie_walk_animation;
    TransformAnimationPlayer* magpie_idle_animation;
    TransformAnimationPlayer* magpie_steal_animation;

    // Transforms for guard animations
    std::vector< Scene::Transform* > guard_model_patrol_transforms;
    std::vector< Scene::Transform* > guard_model_chase_transforms;
    std::vector< Scene::Transform* > guard_model_alert_transforms;

    // Guard Animation players
    TransformAnimationPlayer* current_guard_animation = nullptr;
    TransformAnimationPlayer* guard_patrol_animation;
    TransformAnimationPlayer* guard_chase_animation;
    TransformAnimationPlayer* guard_alert_animation;

    // Basically manages what animations are playing
    std::list< TransformAnimationPlayer* > current_animations;

    // Use one main transform and swap it to point between
    // one of the three other specific transforms
    Scene::Transform *player_trans = nullptr;
    Scene::Transform *player_idle_trans = nullptr;
    Scene::Transform *player_walk_trans = nullptr;
    Scene::Transform *player_steal_trans = nullptr;

    // Use one main transform and swap it to point between
    // one of the three other specific transforms
    Scene::Transform *guard_trans = nullptr;
    Scene::Transform *guard_patrol_trans = nullptr;
    Scene::Transform *guard_chase_trans = nullptr;
    Scene::Transform *guard_alert_trans = nullptr;

    // Off screen position to place the guard and player meshes that are not being used
    glm::vec3 OFF_SCREEN_POS(-10000.0f, -10000.0f, -10000.0f);

    MagpieGame game;

    // BUILDING TILES
    Load< MeshBuffer > scenery_meshes(LoadTagDefault, [](){
        return new MeshBuffer(data_path("building_tiles.pnc"));
    });

    Load< GLuint > scenery_meshes_for_vertex_color_program(LoadTagDefault, [](){
        return new GLuint(scenery_meshes->make_vao_for_program(vertex_color_program->program));
    });

    // PLAYER IDLE
    Load< MeshBuffer > magpie_player_idle_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie_idle.pnc"));
    });

    Load< GLuint > magpie_player_idle_mesh_for_vertex_color_program(LoadTagDefault, [](){
        return new GLuint(magpie_player_idle_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > player_idle_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("player_idle.tanim"));
    });

    // PLAYER WALKING
    Load< MeshBuffer > magpie_player_walk_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie_walk.pnc"));
    });

    Load< GLuint > magpie_player_walk_mesh_for_vertex_color_program(LoadTagDefault, [](){
        return new GLuint(magpie_player_walk_mesh->make_vao_for_program(vertex_color_program->program));
    });
    
    Load< TransformAnimation > player_walk_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("player_walk.tanim"));
    });

    // PLAYER STEALING
    Load< MeshBuffer > magpie_player_steal_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie_steal.pnc"));
    });

    Load< GLuint > magpie_player_steal_mesh_for_vertex_color_program(LoadTagDefault, [](){
        return new GLuint(magpie_player_steal_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > player_steal_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("player_steal.tanim"));
    });

    // GUARD PATROLING
    Load< MeshBuffer > guard_dog_patrol_mesh(LoadTagDefault, []() {
        return new MeshBuffer(data_path("guardDog_patrol.pnc"));
    });

    Load< GLuint > guard_dog_patrol_vertex_color_program(LoadTagDefault, []() {
        return new GLuint(guard_dog_patrol_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > guard_dog_patrol_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("guardDog_patrol.tanim"));
    });

    // GUARD CHASING
    Load< MeshBuffer > guard_dog_chase_mesh(LoadTagDefault, []() {
        return new MeshBuffer(data_path("guardDog_chase.pnc"));
    });

    Load< GLuint > guard_dog_chase_vertex_color_program(LoadTagDefault, []() {
        return new GLuint(guard_dog_chase_mesh->make_vao_for_program(vertex_color_program->program));
    });
    
    Load< TransformAnimation > guard_dog_chase_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("guardDog_chase.tanim"));
    });

    // GUARD ALERT
    Load< MeshBuffer > guard_dog_alert_mesh(LoadTagDefault, []() {
        return new MeshBuffer(data_path("guardDog_alert.pnc"));
    });

    Load< GLuint > guard_dog_alert_vertex_color_program(LoadTagDefault, []() {
        return new GLuint(guard_dog_alert_mesh->make_vao_for_program(vertex_color_program->program));
    });
    
    Load< TransformAnimation > guard_dog_alert_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("guardDog_alert.tanim"));
    });

    void MagpieGameMode::init_scene() {
        // Single Program for drawing
        Scene::Object::ProgramInfo vertex_color_program_info;
        vertex_color_program_info.program = vertex_color_program->program;
        vertex_color_program_info.mvp_mat4 = vertex_color_program->object_to_clip_mat4;
        vertex_color_program_info.mv_mat4x3 = vertex_color_program->object_to_light_mat4x3;
        vertex_color_program_info.itmv_mat3 = vertex_color_program->normal_to_light_mat3;

        // Vaos that the vertex color program will use
        std::map< std::string, GLuint > vertex_color_vaos = {
            {"scenery", *scenery_meshes_for_vertex_color_program},
            {"magpieWalk", *magpie_player_walk_mesh_for_vertex_color_program},
            {"magpieIdle", *magpie_player_idle_mesh_for_vertex_color_program},
            {"magpieSteal", *magpie_player_steal_mesh_for_vertex_color_program},
            {"guardPatrol", *guard_dog_patrol_vertex_color_program},
            {"guardChase", *guard_dog_chase_vertex_color_program},
            {"guardAlert", *guard_dog_alert_vertex_color_program}
        };

        // Get the level loading object
        Magpie::LevelLoader level_pixel_data;
        currFloor = level_pixel_data.load(data_path("demo-map-simple.lvl"), &game, &scene, scenery_meshes.value, [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("scenery")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = scenery_meshes->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
            return obj;
        });

        // Load in the magpie walk mesh
        scene.load(data_path("magpie_walk.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("magpieWalk")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = magpie_player_walk_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });
        player_walk_trans = scene.look_up("magpieWalk_GRP");
        assert(player_walk_trans != nullptr);
        player_walk_trans->position = OFF_SCREEN_POS;

        // Load in the magpie idle mesh
        scene.load(data_path("magpie_idle.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("magpieIdle")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = magpie_player_idle_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });
        player_idle_trans = scene.look_up("magpieIdle_GRP");
        assert(player_idle_trans != nullptr);
        player_idle_trans->position = OFF_SCREEN_POS;

        // Load in the magpie idle mesh
        scene.load(data_path("magpie_steal.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("magpieSteal")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = magpie_player_steal_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });
        player_steal_trans = scene.look_up("magpieSteal_GRP");
        assert(player_steal_trans != nullptr);
        player_steal_trans->position = OFF_SCREEN_POS;

        // Load in the guard patrol mesh
        scene.load(data_path("guardDog_patrol.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("guardPatrol")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = guard_dog_patrol_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });
        guard_patrol_trans = scene.look_up("guardDogPatrol_GRP");
        assert(guard_patrol_trans != nullptr);
        guard_patrol_trans->position = OFF_SCREEN_POS;

        // Load in the guard patrol mesh
        scene.load(data_path("guardDog_chase.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("guardChase")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = guard_dog_chase_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });
        guard_chase_trans = scene.look_up("guardDogChase_GRP");
        assert(guard_chase_trans != nullptr);
        guard_chase_trans->position = OFF_SCREEN_POS;

        // Load in the guard patrol mesh
        scene.load(data_path("guardDog_alert.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("guardAlert")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = guard_dog_alert_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });
        guard_alert_trans = scene.look_up("guardDogAlert_GRP");
        assert(guard_alert_trans != nullptr);
        guard_alert_trans->position = OFF_SCREEN_POS;


        // Set the general transform pointers
        player_trans = player_idle_trans;
        guard_trans = guard_patrol_trans;

        // Move the transforms into the camera's view
        player_trans->position.x = 1.0f;
        player_trans->position.y = 1.0f;
        player_trans->position.z = 0.0f;
        guard_trans->position.x = 3.0f;
        guard_trans->position.y = 3.0f;
        guard_trans->position.z = 0.0f;

        // We are just using this for the camera positioning
        scene.load(data_path("camera_transform.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            // Save resources
            scene.delete_transform(t);
        });

        //look up various transforms for animations
        std::unordered_map< std::string, Scene::Transform * > name_to_transform;

        for (Scene::Transform *t = scene.first_transform; t != nullptr; t = t->alloc_next) {
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

        for (auto const &name : player_steal_tanim->names) {
            auto f = name_to_transform.find(name);
            if (f == name_to_transform.end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                player_model_steal_transforms.emplace_back(nullptr);
            } else {
                player_model_steal_transforms.emplace_back(f->second);
            }
        }

        for (auto const &name : guard_dog_patrol_tanim->names) {
            auto f = name_to_transform.find(name);
            if (f == name_to_transform.end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                guard_model_patrol_transforms.emplace_back(nullptr);
            } else {
                guard_model_patrol_transforms.emplace_back(f->second);
            }
        }

        for (auto const &name : guard_dog_chase_tanim->names) {
            auto f = name_to_transform.find(name);
            if (f == name_to_transform.end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                guard_model_chase_transforms.emplace_back(nullptr);
            } else {
                guard_model_chase_transforms.emplace_back(f->second);
            }
        }

        for (auto const &name : guard_dog_alert_tanim->names) {
            auto f = name_to_transform.find(name);
            if (f == name_to_transform.end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                guard_model_alert_transforms.emplace_back(nullptr);
            } else {
                guard_model_alert_transforms.emplace_back(f->second);
            }
        }

        
        magpie_walk_animation = new TransformAnimationPlayer(*player_walk_tanim, player_model_walk_transforms, 1.0f, true);
        magpie_idle_animation = new TransformAnimationPlayer(*player_idle_tanim, player_model_idle_transforms, 1.0f, true);
        magpie_steal_animation = new TransformAnimationPlayer(*player_steal_tanim, player_model_steal_transforms, 1.0f, false);

        guard_patrol_animation = new TransformAnimationPlayer(*guard_dog_patrol_tanim, guard_model_patrol_transforms, 2.0f, true);
        guard_chase_animation = new TransformAnimationPlayer(*guard_dog_chase_tanim, guard_model_chase_transforms, 1.0f, true);
        guard_alert_animation = new TransformAnimationPlayer(*guard_dog_alert_tanim, guard_model_alert_transforms, 1.0f, false);

        // Start walk animation
        current_player_animation = magpie_idle_animation;
        current_guard_animation = guard_patrol_animation;
        //current_animations.push_back(magpie_idle_animation);
        //current_animations.push_back(guard_patrol_animation);

        //look up the camera:
        for (Scene::Camera *c = scene.first_camera; c != nullptr; c = c->alloc_next) {
            if (c->transform->name == "Camera") {
                if (camera) throw std::runtime_error("Multiple 'Camera' objects in scene.");
                camera = c;
            }
        }
        if (!camera) throw std::runtime_error("No 'Camera' camera in scene.");

        camera_trans = scene.new_transform();
        camera_trans->rotation = glm::angleAxis(glm::radians(360.0f), glm::vec3(0.0, 0.0, 1.0));
        camera->transform->parent = camera_trans;

        //look up the spotlight:
        for (Scene::Lamp *l = scene.first_lamp; l != nullptr; l = l->alloc_next) {
            if (l->transform->name == "Spot") {
                if (spot) throw std::runtime_error("Multiple 'Spot' objects in scene.");
                if (l->type != Scene::Lamp::Spot) throw std::runtime_error("Lamp 'Spot' is not a spotlight.");
                spot = l;
            }
        }
        if (!spot) throw std::runtime_error("No 'Spot' spotlight in scene.");
    };

    MagpieGameMode::MagpieGameMode() {
        init_scene();
        Navigation::getInstance().loadGrid(&currFloor);
        initEntities();
    };

    MagpieGameMode::~MagpieGameMode() {

    };

    void MagpieGameMode::initEntities() {

        entityFactoryMap = {
                {1, ENTITY_FACTORY(Player)},
                {2, ENTITY_FACTORY(Guard)}
        };

        game.player = ENTITY_FACTORY(Player)(0, 0, player_trans);

        game.entities.push_back(ENTITY_FACTORY(Guard)(1, 1, guard_trans));
    }

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

        if (game.player->getAgent()->state == Agent::STATE::IDLE) {
            printf("IDLE\n");
        }
        else if (game.player->getAgent()->state == Agent::STATE::WALKING) {
            printf("WALKING\n");
        }

        if (current_player_animation == magpie_idle_animation) {
            printf("IDLE ANiMAitioN\n");
        }

        if (current_player_animation == magpie_walk_animation) {
            printf("WALK ANiMAitioN\n");
        }

        if (game.player->getAgent()->state == Agent::STATE::IDLE &&
            current_player_animation == magpie_walk_animation) {
            magpie_idle_animation->reset();
            current_player_animation = magpie_idle_animation;
            glm::vec3 position = player_trans->position;
            glm::quat rotation = player_trans->rotation;
            player_idle_trans->position = position;
            player_idle_trans->rotation = rotation;
            player_steal_trans->position = OFF_SCREEN_POS;
            player_walk_trans->position = OFF_SCREEN_POS;
            player_trans = player_idle_trans;
        }
        /*
        else if (game.player->getAgent()->state == Agent::STATE::WALKING &&
            current_player_animation != magpie_walk_animation) {
            printf("Starting player walk animation\n");
            magpie_walk_animation->reset();
            current_player_animation = magpie_walk_animation;
            // Swap the meshes
            glm::vec3 position = player_trans->position;
            glm::quat rotation = player_trans->rotation;
            player_walk_trans->position = position;
            player_walk_trans->rotation = rotation;
            player_steal_trans->position = OFF_SCREEN_POS;
            player_idle_trans->position = OFF_SCREEN_POS;
            player_trans = player_walk_trans;
        }
        */
        

        magMoveCountdown -= elapsed;

        if (current_player_animation != nullptr) {
            current_player_animation->update(elapsed);
            if (current_player_animation->done()) {
                if (current_player_animation == magpie_steal_animation) {
                    magpie_steal_animation->reset();
                    magpie_idle_animation->reset();
                    current_player_animation = magpie_idle_animation;
                    // Swap the meshes
                    glm::vec3 position = player_trans->position;
                    glm::quat rotation = player_trans->rotation;
                    player_idle_trans->position = position;
                    player_idle_trans->rotation = rotation;
                    player_steal_trans->position = OFF_SCREEN_POS;
                    player_walk_trans->position = OFF_SCREEN_POS;
                    player_trans = player_idle_trans;
                }
            }
        }
    

        if (current_guard_animation != nullptr) {
            current_guard_animation->update(elapsed);
            if (current_guard_animation->done()) {
                guard_patrol_animation->reset();
                current_guard_animation = guard_chase_animation;
                guard_alert_animation->reset();
                // Swap the meshes
                glm::vec3 position = guard_trans->position;
                guard_patrol_trans->position = position;
                guard_alert_trans->position = OFF_SCREEN_POS;
                guard_chase_trans->position = OFF_SCREEN_POS;
                guard_trans = guard_patrol_trans;
            }
        }

        
        //for (auto ca = current_animations.begin(); ca != current_animations.end(); /* later */ ) {
        //    (*ca)->update(elapsed);
        //    if ((*ca)->done()) {
        //        auto old = ca;
        //        ++ca;
        //        current_animations.erase(old);
        //    } else {
        //        ++ca;
        //    }
        //}

        game.player->update(elapsed);

        for (Entity* entity: game.entities) {
            entity->update(elapsed);
        }

        camera_trans->position.x = player_trans->position.x;
        camera_trans->position.y = player_trans->position.y;
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

        glm::uvec2 pickedTile = currFloor.tileCoord(pointOfIntersect);

        return pickedTile;
    };

    bool MagpieGameMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
        //ignore any keys that are the result of automatic key repeat:
        if (evt.type == SDL_KEYDOWN && evt.key.repeat) {
            return false;
        }

        //TODO: WRITE THIS
        if (evt.type == SDL_KEYDOWN && evt.key.repeat == 0) {
            if (evt.key.keysym.scancode == SDL_SCANCODE_M) {
                //put away inventory if needed
                //call menu
                return true;
            }
            else if (evt.key.keysym.scancode == SDL_SCANCODE_I) {
                //put away map if needed
                //call inventory
                return true;
            }
        }

        //TODO: WRITE THIS
        if (evt.type == SDL_MOUSEMOTION) {
            //call mousepick with mouse.x and mouse.y to get the path of 
            //glm::uvec2 endpt = mousePick(evt.motion.x, evt.motion.y, window_size.x, window_size.y, 0, camera, "prototype");
            //highlightPath = ;
            return true;
        }

        if (evt.type == SDL_MOUSEBUTTONDOWN) {
            if (evt.button.button == SDL_BUTTON_LEFT) {
                glm::uvec2 clickedTile = mousePick(evt.button.x, evt.button.y, 
                                        window_size.x, window_size.y, 0, camera, "prototype");
                std::cout << "clickedTile.x is "<< clickedTile.x << "and clickTile.y is "<< clickedTile.y << std::endl;
                if (clickedTile.x<currFloor.rows && clickedTile.y<currFloor.cols) { //ignore (-1, -1)/error
                    // Check that the space is a 1 in the movement grid
                    if (currFloor.map[clickedTile.x][clickedTile.y] == true && current_player_animation != magpie_steal_animation) {
                        Magpie::game.player->setDestination(clickedTile);
                        
                    } else {
                        // TODO: ACTUALLY MAKE THIS DO SOMETHING OTHER THAN DELETE THINGS
                        // AND ADJUST FOR THE HEIGHT OF OBJECTS
                        if (currFloor.interaction_map[clickedTile.x][clickedTile.y] == true &&
                            std::abs((int)player_trans->position.x - (int)clickedTile.x) <= 1 && 
                            std::abs((int)player_trans->position.y - (int)clickedTile.y) <= 1) {
                            Scene::Object* grabbed_item = game.remove_placed_item(clickedTile.x, clickedTile.y);
                            if (grabbed_item != nullptr) {
                                currFloor.interaction_map[clickedTile.x][clickedTile.y] = false;
                                scene.delete_object(grabbed_item);
                                // Play animation
                                magpie_steal_animation->reset();
                                current_player_animation = magpie_steal_animation;
                                glm::vec3 position = player_trans->position;
                                player_steal_trans->position = position;
                                player_walk_trans->position = OFF_SCREEN_POS;
                                player_idle_trans->position = OFF_SCREEN_POS;
                                player_trans = player_steal_trans;

                                // Rotate the player based on where the item is
                                if ((int)player_trans->position.y < (int)clickedTile.y) {
                                    if ((int)player_trans->position.x < (int)clickedTile.x) {
                                        player_trans->rotation *= glm::angleAxis(glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                                    }
                                    else if ((int)player_trans->position.x > (int)clickedTile.x) {
                                        player_trans->rotation *= glm::angleAxis(glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                                    }
                                    else {
                                        player_trans->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                                    }
                                }
                                if ((int)player_trans->position.y > (int)clickedTile.y) {
                                    if ((int)player_trans->position.x < (int)clickedTile.x) {
                                        player_trans->rotation *= glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                                    }
                                    else if ((int)player_trans->position.x > (int)clickedTile.x) {
                                        player_trans->rotation *= glm::angleAxis(glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                                    }
                                    else {
                                        //player_trans->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                                    }
                                }
                                else
                                {
                                    if ((int)player_trans->position.x < (int)clickedTile.x) {
                                        player_trans->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                                    }
                                    else if ((int)player_trans->position.x > (int)clickedTile.x) {
                                        player_trans->rotation *= glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                                    }
                                }
                            }
                        }
                    }
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
            scene.draw(camera);

            //TODO: highlight magpie path! WHAT STORES THE TRANSFORMS OF ALL THE FLOOR TILES IN MAP?
            //make highlightedPath squares seem brighter than the ones around it
            /*
            for (uint32_t i=0; i<highlightPath.size(); i++) {
                ;
            }
            */
        }

        GL_ERRORS();
    };
}