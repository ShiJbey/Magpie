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

    MagpieGame game;
    Scene::Camera *camera = nullptr;
    Scene::Transform *camera_trans = nullptr;

    // Use one main transform and swap it to point between
    // one of the three other specific transforms
    Scene::Transform *player_trans = nullptr;
    Scene::Transform *player_idle_trans = nullptr;
    Scene::Transform *player_walk_trans = nullptr;
    Scene::Transform *player_steal_trans = nullptr;

    // Tansforms for player animations
    std::vector< Scene::Transform* > player_model_walk_transforms;
    std::vector< Scene::Transform* > player_model_steal_transforms;
    std::vector< Scene::Transform* > player_model_idle_transforms;

    // Player Animation players
    TransformAnimationPlayer* current_player_animation = nullptr;
    TransformAnimationPlayer* magpie_walk_animation = nullptr;
    TransformAnimationPlayer* magpie_idle_animation = nullptr;
    TransformAnimationPlayer* magpie_steal_animation = nullptr;

    // Use one main transform and swap it to point between
    // one of the three other specific transforms
    Scene::Transform *guard_trans = nullptr;
    Scene::Transform *guard_patrol_trans = nullptr;
    Scene::Transform *guard_chase_trans = nullptr;
    Scene::Transform *guard_alert_trans = nullptr;
    Scene::Transform *guard_confused_trans = nullptr;
    Scene::Transform *guard_cautious_trans = nullptr;
    Scene::Transform *guard_idle_trans = nullptr;

    // Transforms for guard animations
    std::vector< Scene::Transform* > guard_model_idle_transforms;
    std::vector< Scene::Transform* > guard_model_patrol_transforms;
    std::vector< Scene::Transform* > guard_model_chase_transforms;
    std::vector< Scene::Transform* > guard_model_alert_transforms;
    std::vector< Scene::Transform* > guard_model_cautious_transforms;
    std::vector< Scene::Transform* > guard_model_confused_transforms;

    // Guard Animation players
    TransformAnimationPlayer* current_guard_animation = nullptr;
    TransformAnimationPlayer* guard_idle_animation = nullptr;
    TransformAnimationPlayer* guard_patrol_animation = nullptr;
    TransformAnimationPlayer* guard_chase_animation = nullptr;
    TransformAnimationPlayer* guard_alert_animation = nullptr;
    TransformAnimationPlayer* guard_cautious_animation = nullptr;
    TransformAnimationPlayer* guard_confused_animation = nullptr;

    // Off screen position to place the guard and player meshes that are not being used
    glm::vec3 OFF_SCREEN_POS(-10000.0f, -10000.0f, -10000.0f);

    // BUILDING TILES
    Load< MeshBuffer > building_meshes(LoadTagDefault, [](){
        return new MeshBuffer(data_path("levels/building_tiles.pnc"));
    });

    Load< GLuint > building_meshes_vao(LoadTagDefault, [](){
        return new GLuint(building_meshes->make_vao_for_program(vertex_color_program->program));
    });

    // PLAYER IDLE
    Load< MeshBuffer > magpie_idle_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie/magpie_idle.pnc"));
    });

    Load< GLuint > magpie_idle_mesh_vao(LoadTagDefault, [](){
        return new GLuint(magpie_idle_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > magpie_idle_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("magpie/magpie_idle.tanim"));
    });

    // PLAYER WALKING
    Load< MeshBuffer > magpie_walk_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie/magpie_walk.pnc"));
    });

    Load< GLuint > magpie_walk_mesh_vao(LoadTagDefault, [](){
        return new GLuint(magpie_walk_mesh->make_vao_for_program(vertex_color_program->program));
    });
    
    Load< TransformAnimation > magpie_walk_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("magpie/magpie_walk.tanim"));
    });

    // PLAYER STEALING
    Load< MeshBuffer > magpie_steal_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie/magpie_steal.pnc"));
    });

    Load< GLuint > magpie_steal_mesh_vao(LoadTagDefault, [](){
        return new GLuint(magpie_steal_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > magpie_steal_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("magpie/player_steal.tanim"));
    });

    // GUARD PATROLING
    Load< MeshBuffer > guard_dog_patrol_mesh(LoadTagDefault, []() {
        return new MeshBuffer(data_path("guardDog/guardDog_patrol.pnc"));
    });

    Load< GLuint > guard_dog_patrol_vao(LoadTagDefault, []() {
        return new GLuint(guard_dog_patrol_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > guard_dog_patrol_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("guardDog/guardDog_patrol.tanim"));
    });

    // GUARD CHASING
    Load< MeshBuffer > guard_dog_chase_mesh(LoadTagDefault, []() {
        return new MeshBuffer(data_path("guardDog/guardDog_chase.pnc"));
    });

    Load< GLuint > guard_dog_chase_vao(LoadTagDefault, []() {
        return new GLuint(guard_dog_chase_mesh->make_vao_for_program(vertex_color_program->program));
    });
    
    Load< TransformAnimation > guard_dog_chase_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("guardDog/guardDog_chase.tanim"));
    });

    // GUARD ALERT
    Load< MeshBuffer > guard_dog_alert_mesh(LoadTagDefault, []() {
        return new MeshBuffer(data_path("guardDog/guardDog_alert.pnc"));
    });

    Load< GLuint > guard_dog_alert_vao(LoadTagDefault, []() {
        return new GLuint(guard_dog_alert_mesh->make_vao_for_program(vertex_color_program->program));
    });
    
    Load< TransformAnimation > guard_dog_alert_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("guardDog/guardDog_alert.tanim"));
    });

    // GUARD CAUTIOUS
    Load< MeshBuffer > guard_dog_cautious_mesh(LoadTagDefault, []() {
        return new MeshBuffer(data_path("guardDog/guardDog_cautious.pnc"));
    });

    Load< GLuint > guard_dog_cautious_vao(LoadTagDefault, []() {
        return new GLuint(guard_dog_cautious_mesh->make_vao_for_program(vertex_color_program->program));
    });
    
    Load< TransformAnimation > guard_dog_cautious_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("guardDog/guardDog_cautious.tanim"));
    });

    // GUARD CONFUSED
    Load< MeshBuffer > guard_dog_confused_mesh(LoadTagDefault, []() {
        return new MeshBuffer(data_path("guardDog/guardDog_confused.pnc"));
    });

    Load< GLuint > guard_dog_confused_vao(LoadTagDefault, []() {
        return new GLuint(guard_dog_confused_mesh->make_vao_for_program(vertex_color_program->program));
    });
    
    Load< TransformAnimation > guard_dog_confused_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("guardDog/guardDog_confused.tanim"));
    });

    // GUARD IDLE
    Load< MeshBuffer > guard_dog_idle_mesh(LoadTagDefault, []() {
        return new MeshBuffer(data_path("guardDog/guardDog_idle.pnc"));
    });

    Load< GLuint > guard_dog_idle_vao(LoadTagDefault, []() {
        return new GLuint(guard_dog_idle_mesh->make_vao_for_program(vertex_color_program->program));
    });
    
    Load< TransformAnimation > guard_dog_idle_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("guardDog/guardDog_idle.tanim"));
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
            {"scenery", *building_meshes_vao},
            {"magpieWalk", *magpie_walk_mesh_vao},
            {"magpieIdle", *magpie_idle_mesh_vao},
            {"magpieSteal", *magpie_steal_mesh_vao},
            {"guardPatrol", *guard_dog_patrol_vao},
            {"guardChase", *guard_dog_chase_vao},
            {"guardAlert", *guard_dog_alert_vao},
            {"guardCautious", *guard_dog_cautious_vao},
            {"guardConfused", *guard_dog_confused_vao},
            {"guardIdle", *guard_dog_idle_vao}
        };

        // Get the level loading object
        Magpie::LevelLoader level_pixel_data;
        currFloor = level_pixel_data.load(data_path("levels/floorplans/demo-map-simple.lvl"), &game, &scene, building_meshes.value, [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("scenery")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = building_meshes->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
            return obj;
        });

        // Load in the magpie walk mesh
        scene.load(data_path("magpie/magpie_walk.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("magpieWalk")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = magpie_walk_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });
        player_walk_trans = scene.look_up("magpieWalk_GRP");
        assert(player_walk_trans != nullptr);
        player_walk_trans->position = OFF_SCREEN_POS;

        // Load in the magpie idle mesh
        scene.load(data_path("magpie/magpie_idle.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("magpieIdle")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = magpie_idle_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });
        player_idle_trans = scene.look_up("magpieIdle_GRP");
        assert(player_idle_trans != nullptr);
        player_idle_trans->position = OFF_SCREEN_POS;

        // Load in the magpie steal mesh
        scene.load(data_path("magpie/magpie_steal.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("magpieSteal")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = magpie_steal_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });
        player_steal_trans = scene.look_up("magpieSteal_GRP");
        assert(player_steal_trans != nullptr);
        player_steal_trans->position = OFF_SCREEN_POS;

        // Load in the guard patrol mesh
        scene.load(data_path("guardDog/guardDog_patrol.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
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

        // Load in the guard chase mesh
        scene.load(data_path("guardDog/guardDog_chase.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
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

        // Load in the guard confused mesh
        scene.load(data_path("guardDog/guardDog_confused.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("guardConfused")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = guard_dog_confused_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });
        guard_confused_trans = scene.look_up("guardDogConfused_GRP");
        assert(guard_confused_trans != nullptr);
        guard_confused_trans->position = OFF_SCREEN_POS;

        // Load in the guard cautious mesh
        scene.load(data_path("guardDog/guardDog_cautious.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("guardCautious")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = guard_dog_cautious_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });
        guard_cautious_trans = scene.look_up("guardDogCautious_GRP");
        assert(guard_cautious_trans != nullptr);
        guard_cautious_trans->position = OFF_SCREEN_POS;

        // Load in the guard idle mesh
        scene.load(data_path("guardDog/guardDog_idle.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("guardIdle")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = guard_dog_idle_mesh->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });
        guard_idle_trans = scene.look_up("guardDogIdle_GRP");
        assert(guard_idle_trans != nullptr);
        guard_idle_trans->position = OFF_SCREEN_POS;

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
        scene.load(data_path("levels/camera_transform.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
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

        for (auto const &name : magpie_walk_tanim->names) {
            auto f = name_to_transform.find(name);
            if (f == name_to_transform.end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                player_model_walk_transforms.emplace_back(nullptr);
            } else {
                player_model_walk_transforms.emplace_back(f->second);
            }
        }

        for (auto const &name : magpie_idle_tanim->names) {
            auto f = name_to_transform.find(name);
            if (f == name_to_transform.end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                player_model_idle_transforms.emplace_back(nullptr);
            } else {
                player_model_idle_transforms.emplace_back(f->second);
            }
        }

        for (auto const &name : magpie_steal_tanim->names) {
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

        for (auto const &name : guard_dog_confused_tanim->names) {
            auto f = name_to_transform.find(name);
            if (f == name_to_transform.end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                guard_model_confused_transforms.emplace_back(nullptr);
            } else {
                guard_model_confused_transforms.emplace_back(f->second);
            }
        }

        for (auto const &name : guard_dog_cautious_tanim->names) {
            auto f = name_to_transform.find(name);
            if (f == name_to_transform.end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                guard_model_cautious_transforms.emplace_back(nullptr);
            } else {
                guard_model_cautious_transforms.emplace_back(f->second);
            }
        }

        for (auto const &name : guard_dog_idle_tanim->names) {
            auto f = name_to_transform.find(name);
            if (f == name_to_transform.end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                guard_model_idle_transforms.emplace_back(nullptr);
            } else {
                guard_model_idle_transforms.emplace_back(f->second);
            }
        }

        magpie_walk_animation = new TransformAnimationPlayer(*magpie_walk_tanim, player_model_walk_transforms, 1.0f, true);
        magpie_idle_animation = new TransformAnimationPlayer(*magpie_idle_tanim, player_model_idle_transforms, 1.0f, true);
        magpie_steal_animation = new TransformAnimationPlayer(*magpie_steal_tanim, player_model_steal_transforms, 1.0f, false);

        guard_patrol_animation = new TransformAnimationPlayer(*guard_dog_patrol_tanim, guard_model_patrol_transforms, 1.0f, true);
        guard_chase_animation = new TransformAnimationPlayer(*guard_dog_chase_tanim, guard_model_chase_transforms, 1.0f, true);
        guard_alert_animation = new TransformAnimationPlayer(*guard_dog_alert_tanim, guard_model_alert_transforms, 1.0f, false);
        guard_confused_animation = new TransformAnimationPlayer(*guard_dog_confused_tanim, guard_model_confused_transforms, 1.0f, true);
        guard_cautious_animation = new TransformAnimationPlayer(*guard_dog_cautious_tanim, guard_model_cautious_transforms, 1.0f, true);
        guard_idle_animation = new TransformAnimationPlayer(*guard_dog_idle_tanim, guard_model_idle_transforms, 1.0f, true);


        // Setanimation
        current_player_animation = magpie_idle_animation;
        current_guard_animation = guard_patrol_animation;

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

    void MagpieGameMode::update(float elapsed) {

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
        glm::vec3 norm_worldDir = glm::normalize(worldDir);
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

    void cast_ray(int mouse_x, int mouse_y, int screen_width, int screen_height, const Scene::Camera* cam) {
        
        // Get the [-1,1] normalized coordinates of the click 
        float normalized_device_x = (2.0f * mouse_x) / screen_width - 1.0f;
        float normalized_device_y = 1.0f - (2.0f * mouse_y) / screen_height;
        float z = 1.0f;

        glm::vec3 ray_nds = glm::vec3(normalized_device_x, normalized_device_y, z);
        // Ray defined in clip space [-1,1] coordinates
        glm::vec4 ray_clip_space = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
        // Converts the above ray to clip space coordinates
        glm::vec4 ray_view_space = glm::inverse(cam->make_projection()) * ray_clip_space;
        // We only need the x and y coordinates for now, change z and w
        ray_view_space.z = -1.0f;
        ray_view_space.w = 0.0f;
        // Convert to  world space by multiplying by the inverse of the view matrix
        glm::vec4 ray_world_space = glm::inverse(cam->transform->make_local_to_world()) * ray_view_space;
        glm::vec3 ray_world;
        ray_world.x = ray_world_space.x;
        ray_world.y = ray_world_space.y;
        ray_world.z = ray_world_space.z;
        // Normalize the final result
        ray_world = glm::normalize(ray_world);


    };

    bool MagpieGameMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
        //ignore any keys that are the result of automatic key repeat:
        if (evt.type == SDL_KEYDOWN && evt.key.repeat) {
            return false;
        }

        if (evt.type == SDL_MOUSEBUTTONUP) {
            if (evt.button.button == SDL_BUTTON_LEFT) {

                std::cout << "X:" << evt.button.x << " Y:" << evt.button.y << std::endl;
                cast_ray(evt.button.x, evt.button.y, window_size.x, window_size.y, camera);
                //TODO: get x and y of mouse click and figure out which tile it is
                glm::uvec2 clickedTile = mousePick(evt.button.x, evt.button.y, 
                                        window_size.x, window_size.y, 0, camera, "prototype");
                //std::cout << "clickedTile.x is "<< clickedTile.x << "and clickTile.y is "<< clickedTile.y << std::endl;
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
        }

        GL_ERRORS();
    };
}