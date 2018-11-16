#include "MagpieGamemode.hpp"

#include "Clickable.hpp"
#include "AnimationManager.hpp"
#include "TransformAnimation.hpp"
#include "load_level.hpp"
#include "MagpieGame.hpp"

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
#include "highlight_program.hpp"
#include "transparent_program.hpp"
#include "depth_program.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <map>
#include <cstddef>
#include <random>
#include <unordered_map>
#include <cstdlib>

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

    Load< GLuint > highlighted_building_meshes_vao(LoadTagDefault, [](){
        return new GLuint(building_meshes->make_vao_for_program(highlight_program->program));
    });

    Load< GLuint > transparent_building_meshes_vao(LoadTagDefault, [](){
        return new GLuint(building_meshes->make_vao_for_program(transparent_program->program));
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

    Scene::Object::ProgramInfo vertex_color_program_info;
    Scene::Object::ProgramInfo highlight_program_info;
    Scene::Object::ProgramInfo transparent_program_info;

    void MagpieGameMode::init_scene() {
        // Basic Vertex Color Program
        vertex_color_program_info.program = vertex_color_program->program;
        vertex_color_program_info.mvp_mat4 = vertex_color_program->object_to_clip_mat4;
        vertex_color_program_info.mv_mat4x3 = vertex_color_program->object_to_light_mat4x3;
        vertex_color_program_info.itmv_mat3 = vertex_color_program->normal_to_light_mat3;

        // Program for highlighting the path
        highlight_program_info.program = highlight_program->program;
        highlight_program_info.mvp_mat4 = highlight_program->object_to_clip_mat4;
        highlight_program_info.mv_mat4x3 = highlight_program->object_to_light_mat4x3;
        highlight_program_info.itmv_mat3 = highlight_program->normal_to_light_mat3;

        // Program for making walls transparent
        transparent_program_info.program = transparent_program->program;
        transparent_program_info.mvp_mat4 = transparent_program->object_to_clip_mat4;
        transparent_program_info.mv_mat4x3 = transparent_program->object_to_light_mat4x3;
        transparent_program_info.itmv_mat3 = transparent_program->normal_to_light_mat3;

        

        // Vaos that the vertex color program will use
        std::map< std::string, GLuint > vertex_color_vaos = {
            {"buildingTiles", *building_meshes_vao},
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
        level_pixel_data.load(data_path("demo_map_flipped.lvl"), &game, &scene, building_meshes.value, [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos.find("buildingTiles")->second;
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

        // Load in the guard alert mesh
        scene.load(data_path("guardDog/guardDog_alert.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
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

        // Move the transforms into the camera's view
        guard_trans = guard_patrol_trans;
        guard_trans->position.x = 0.0f;
        guard_trans->position.y = 0.0f;
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


        magpie_idle_animation = new TransformAnimationPlayer(*magpie_idle_tanim, player_model_idle_transforms, 1.0f, true);
        magpie_walk_animation = new TransformAnimationPlayer(*magpie_walk_tanim, player_model_walk_transforms, 1.0f, true);
        magpie_steal_animation = new TransformAnimationPlayer(*magpie_steal_tanim, player_model_steal_transforms, 1.0f, false);

        game.player.get_animation_manager()->add_state(new AnimationState(player_idle_trans, magpie_idle_animation));
        game.player.get_animation_manager()->add_state(new AnimationState(player_walk_trans, magpie_walk_animation));
        game.player.get_animation_manager()->add_state(new AnimationState(player_steal_trans, magpie_steal_animation));
        game.player.set_transform(game.player.get_animation_manager()->init(game.player.get_position(), (uint32_t)Player::STATE::IDLE));
        if (game.player.get_transform() == nullptr) {
            printf("FUCK\n");
        }
        game.player.set_position(glm::vec3(7.0f, 7.0f, 0.0f));

        guard_patrol_animation = new TransformAnimationPlayer(*guard_dog_patrol_tanim, guard_model_patrol_transforms, 1.0f, true);
        guard_chase_animation = new TransformAnimationPlayer(*guard_dog_chase_tanim, guard_model_chase_transforms, 1.0f, true);
        guard_alert_animation = new TransformAnimationPlayer(*guard_dog_alert_tanim, guard_model_alert_transforms, 1.0f, false);
        guard_confused_animation = new TransformAnimationPlayer(*guard_dog_confused_tanim, guard_model_confused_transforms, 1.0f, true);
        guard_cautious_animation = new TransformAnimationPlayer(*guard_dog_cautious_tanim, guard_model_cautious_transforms, 1.0f, true);
        guard_idle_animation = new TransformAnimationPlayer(*guard_dog_idle_tanim, guard_model_idle_transforms, 1.0f, true);

        // Set animation
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
        camera_trans->rotation = glm::angleAxis(glm::radians(270.0f), glm::vec3(0.0, 0.0, 1.0));
        camera->transform->parent = camera_trans;
    };

    MagpieGameMode::MagpieGameMode() {
        init_scene();
        Navigation::getInstance().set_movement_matrix(game.current_level->get_movement_matrix());
    };

    MagpieGameMode::~MagpieGameMode() {
        // Do Nothing
    };

    void MagpieGameMode::highlight_path_tiles() {
        std::vector< FloorTile* >* highlighted_tiles = game.current_level->get_highlighted_tiles();
        if (!highlighted_tiles->empty()) {
            for (uint32_t i = 0; i < highlighted_tiles->size(); i++) {
                (*highlighted_tiles)[i]->scene_object->programs[Scene::Object::ProgramTypeDefault] = vertex_color_program_info;
                (*highlighted_tiles)[i]->scene_object->programs[Scene::Object::ProgramTypeDefault].vao = *building_meshes_vao;
            }
        }
        highlighted_tiles->clear();
        
        std::vector<glm::vec2> path = game.player.get_path();
        FloorTile**** floor_matrix = game.current_level->get_floor_matrix();
        for (uint32_t i = 0; i < path.size(); i++) {
            //(*floor_matrix)[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->active = false;
            //(*floor_matrix)[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->programs[Scene::Object::ProgramTypeDefault] = highlight_program_info;
            //(*floor_matrix)[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->programs[Scene::Object::ProgramTypeDefault].vao = *highlighted_building_meshes_vao;
            highlighted_tiles->push_back((*floor_matrix)[(uint32_t)path[i].x][(uint32_t)path[i].y]);
        }
    }

    void MagpieGameMode::update(float elapsed) {

        game.player.update(elapsed);

        for (auto it = game.guards.begin(); it != game.guards.end(); it++) {
            it->update(elapsed);
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
        
        camera_trans->position.x = game.player.get_position().x;
        camera_trans->position.y = game.player.get_position().y;

       // highlight_path_tiles();
    };

    //from this tutorial: http://antongerdelan.net/opengl/raycasting.html
    glm::uvec2 MagpieGameMode::mousePick(int mouseX, int mouseY, int screenWidth, int screenHeight, int floorHeight, const Scene::Camera* cam) {
        glm::mat4 camLocalToWorld = cam->transform->make_local_to_world(); 

        float halfImageHeight = cam->near*std::tan(cam->fovy/2.0f);
        float halfImageWidth = cam->aspect*halfImageHeight;

        //3d Normalized device coords
        float normDeviceX = (2.0f * mouseX) / screenWidth - 1.0f;
        float normDeviceY = 1.0f - (2.0f * mouseY) / screenHeight;

        glm::vec3 localOrigin = glm::vec3(normDeviceX*halfImageWidth, normDeviceY*halfImageHeight, -cam->near);
        glm::vec3 worldOrigin = camLocalToWorld*glm::vec4(localOrigin, 1.0f);
        glm::vec3 worldDir = camLocalToWorld*glm::vec4(localOrigin, 0.0f); //unnormalized dir
        // float dist = worldOrigin.z - floorHeight;

        if (worldDir.z>=0.0f) { //discard all rays going away from floor
            return glm::uvec2(-1,-1);
        }
        //want to solve s.t. floorHeight = origin.z + t*worldDir.z
        float t = (floorHeight-worldOrigin.z)/worldDir.z;

        glm::vec3 pointOfIntersect = worldOrigin + t*worldDir;

        glm::uvec2 pickedTile = game.current_level->floor_tile_coord(pointOfIntersect);

        if (!game.current_level->can_move_to(pickedTile.x, pickedTile.y)) {
            // Try collision code
            // TODO:: Change the index for paintings
            for (auto it = game.current_level->get_paintings()->begin(); it != game.current_level->get_paintings()->end(); it++) {
                for (auto paint_iter = it->second.begin(); paint_iter != it->second.end(); paint_iter++) {
                    if (paint_iter->get_boundingbox()->check_intersect(worldOrigin, worldDir)) {
                        paint_iter->steal(&(game.player));
                        paint_iter->on_click();
                        game.player.set_state((uint32_t)Player::STATE::STEALING);
                    }
                }
            }

            for (auto it = game.current_level->get_gems()->begin(); it != game.current_level->get_gems()->end(); it++) {
                for (auto gem_iter = it->second.begin(); gem_iter != it->second.end(); gem_iter++) {
                    if (gem_iter->get_boundingbox()->check_intersect(worldOrigin, worldDir)) {
                        gem_iter->steal(&(game.player));
                        gem_iter->on_click();
                        game.player.set_state((uint32_t)Player::STATE::STEALING);
                    }
                }
            }
        }

        return pickedTile;
    };

    bool MagpieGameMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

        //ignore any keys that are the result of automatic key repeat:
        if (evt.type == SDL_KEYDOWN && evt.key.repeat) {
            return false;
        }


        if (evt.type == SDL_MOUSEBUTTONUP) {
            if (evt.button.button == SDL_BUTTON_LEFT) {
                //std::cout << "Screen Click at (x: " << evt.button.x << ", y: " << evt.button.y << ")" << std::endl;
                
                glm::uvec2 clickedTile = mousePick(evt.button.x, evt.button.y, window_size.x, window_size.y, 0, camera);
                // Check if we can actually move to that tile
                if (game.current_level->can_move_to(clickedTile.x, clickedTile.y)) {
                    std::cout << "Clicked tile is (x: " << clickedTile.x << ", y: "<< clickedTile.y << ")" << std::endl;
                    game.player.setDestination(clickedTile);
                    if (game.player.get_state() == (uint32_t)Player::STATE::IDLE) {
                        game.player.set_state((uint32_t)Player::STATE::WALKING);
                    }
                }
                else {
                    // Check if the click ray collides with any of the item bounding boxes in the scene
                    ///if (game.current_level->interaction_map[clickedTile.x][clickedTile.y] == true &&
                    //        std::abs((int)player_trans->position.x - (int)clickedTile.x) <= 1 && 
                    //        std::abs((int)player_trans->position.y - (int)clickedTile.y) <= 1) {

                        //game.player.set_state((uint32_t)Player::STATE::STEALING);

                    //}
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