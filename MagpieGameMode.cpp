#include "MagpieGamemode.hpp"

#include "AssetLoader.hpp"
#include "Clickable.hpp"
#include "AnimationManager.hpp"
#include "TransformAnimation.hpp"
#include "load_level.hpp"
#include "MagpieGame.hpp"
#include "GameCharacter.hpp"

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

    // Off screen position to place the guard and player meshes that are not being used
    glm::vec3 OFF_SCREEN_POS(-10000.0f, -10000.0f, -10000.0f);

    // Basic Vertex Color Program
    Scene::Object::ProgramInfo vertex_color_program_info;

    // Program for highlighting the path
    Scene::Object::ProgramInfo highlight_program_info;

    // Program for making walls transparent
    Scene::Object::ProgramInfo transparent_program_info;

    MagpieGameMode::MagpieGameMode() {
        init_program_info();
        load_level("demo_map_flipped.lvl");


        //load_all_models();
        //get_all_animation_transforms();
        //Navigation::getInstance().set_movement_matrix(game.get_level()->get_movement_matrix());
    };

    MagpieGameMode::~MagpieGameMode() {
        // Do Nothing
    };

    void MagpieGameMode::update(float elapsed) {
        /*
        // Update the player
        game.player.update(elapsed);

        // Update the position of the guards
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
        */
    };

    bool MagpieGameMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

        //ignore any keys that are the result of automatic key repeat:
        if (evt.type == SDL_KEYDOWN && evt.key.repeat) {
            return false;
        }

        if (evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_LEFT) {
                // Create a ray from the click
                Magpie::Ray click_ray = create_click_ray(evt.button.x, evt.button.y, window_size.x, window_size.y, camera);
                // Call the handle screen click function
                handle_screen_click(click_ray);
        }

        return false;
    };

    void MagpieGameMode::draw(glm::uvec2 const &drawable_size) {
        glViewport(0, 0, drawable_size.x, drawable_size.y);
        
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

    /**
     * Loads a character model identically to how we load scene data.
     * Once the model is loaded it is moved off screen and apointer to the
     * head transform is retured
     */
    Scene::Transform* MagpieGameMode::load_character_model(std::string filename, std::string vao_key, std::string transform_name, 
            Scene::Object::ProgramInfo program_info, const MeshBuffer* mesh_buffer) {
        
        // Load the model
        scene.load(data_path(filename), [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = program_info;
            default_program_info.vao = vertex_color_vaos->find(vao_key)->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = mesh_buffer->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });

        // Get the top level transform of the model, move it, and return it
        Scene::Transform* model_transform = scene.look_up(transform_name);
        assert(model_transform != nullptr);
        model_transform->position = OFF_SCREEN_POS;
        return model_transform;
    };

    /**
     * Initializes each of the program information structs with everything
     * except for the vao which is set when we instantiate models
     */
    void MagpieGameMode::init_program_info() {
        vertex_color_program_info.program = vertex_color_program->program;
        vertex_color_program_info.mvp_mat4 = vertex_color_program->object_to_clip_mat4;
        vertex_color_program_info.mv_mat4x3 = vertex_color_program->object_to_light_mat4x3;
        vertex_color_program_info.itmv_mat3 = vertex_color_program->normal_to_light_mat3;

        highlight_program_info.program = highlight_program->program;
        highlight_program_info.mvp_mat4 = highlight_program->object_to_clip_mat4;
        highlight_program_info.mv_mat4x3 = highlight_program->object_to_light_mat4x3;
        highlight_program_info.itmv_mat3 = highlight_program->normal_to_light_mat3;

        transparent_program_info.program = transparent_program->program;
        transparent_program_info.mvp_mat4 = transparent_program->object_to_clip_mat4;
        transparent_program_info.mv_mat4x3 = transparent_program->object_to_light_mat4x3;
        transparent_program_info.itmv_mat3 = transparent_program->normal_to_light_mat3;
    };

    /**
     * Given amap of all the transforms in the game and a vector of transforms,
     * returns a fector of Transform pointers to transforms with the given names
     */
    std::vector< Scene::Transform* > MagpieGameMode::get_animation_transforms( std::unordered_map< std::string, Scene::Transform * >* name_to_transform, std::vector< std::string > names) {

        std::vector< Scene::Transform* > animation_transforms;

        for (auto const &name : names) {
            auto f = name_to_transform->find(name);
            if (f == name_to_transform->end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                animation_transforms.emplace_back(nullptr);
            } else {
                animation_transforms.emplace_back(f->second);
            }
        }
        
        return animation_transforms;
    };

    /**
     * Gets the animations transforms for all animations loaded into the game
     */
    void MagpieGameMode::get_all_animation_transforms() {
        //look up various transforms for animations
        std::unordered_map< std::string, Scene::Transform * > name_to_transform;
        for (Scene::Transform *t = scene.first_transform; t != nullptr; t = t->alloc_next) {
            auto ret = name_to_transform.insert(std::make_pair(t->name, t));
            if (!ret.second) {
                std::cerr << "WARNING: multiple transforms with the name '" << t->name << "' in scene." << std::endl;
            }
        }

        
        // Get guard animation transforms
        //guard_model_idle_transforms = get_animation_transforms(&name_to_transform, guard_dog_idle_tanim->names);
        //guard_model_patrol_transforms = get_animation_transforms(&name_to_transform, guard_dog_patrol_tanim->names);
        //guard_model_chase_transforms = get_animation_transforms(&name_to_transform, guard_dog_chase_tanim->names);
        //guard_model_alert_transforms = get_animation_transforms(&name_to_transform, guard_dog_alert_tanim->names);
        //guard_model_cautious_transforms = get_animation_transforms(&name_to_transform, guard_dog_cautious_tanim->names);
        //guard_model_confused_transforms = get_animation_transforms(&name_to_transform, guard_dog_confused_tanim->names);
    };

    /**
     * loads all the character models used in the game
     */
    void MagpieGameMode::load_all_models() {
            // Load Player Models
            //player_idle_trans = load_character_model("magpie/magpie_idle.scene", "magpieIdle", "magpieIdle_GRP", vertex_color_program_info, magpie_idle_mesh.value);
            //player_walk_trans = load_character_model("magpie/magpie_walk.scene", "magpieWalk", "magpieWalk_GRP", vertex_color_program_info, magpie_walk_mesh.value);
            //player_steal_trans = load_character_model("magpie/magpie_steal.scene", "magpieSteal", "magpieSteal_GRP", vertex_color_program_info, magpie_steal_mesh.value);
            // Load Guard Models
            //guard_idle_trans = load_character_model("guardDog/guardDog_idle.scene", "guardidle", "guardDogIdle_GRP", vertex_color_program_info, guard_dog_idle_mesh.value);
            //guard_patrol_trans = load_character_model("guardDog/guardDog_patrol.scene", "guardPatrol", "guardDogPatrol_GRP", vertex_color_program_info, guard_dog_patrol_mesh.value);
            //guard_chase_trans = load_character_model("guardDog/guardDog_chase.scene", "guardChase", "guardDogChase_GRP", vertex_color_program_info, guard_dog_chase_mesh.value);
            //guard_cautious_trans = load_character_model("guardDog/guardDog_cautious.scene", "guardCautious", "guardDogCautious_GRP", vertex_color_program_info, guard_dog_cautious_mesh.value);
            //guard_confused_trans = load_character_model("guardDog/guardDog_confused.scene", "guardConfused", "guardDogConfused_GRP", vertex_color_program_info, guard_dog_confused_mesh.value);
            //guard_alert_trans = load_character_model("guardDog/guardDog_alert.scene", "guardAlert", "guardDogAlert_GRP", vertex_color_program_info, guard_dog_alert_mesh.value);
    };

    /**
     * Loads a single animation into the game
     */
    void MagpieGameMode::load_animation() {

    };

    /**
     * Loads and sets pointers for all the animations used in the game
     */
    void MagpieGameMode::load_all_animations() {

        
    };

    /**
     * Creats a new player in the game and:
     * 1. Sets up its animations
     * 2. Places it a starting position
     */
    void MagpieGameMode::create_player(glm::vec3 position) {
        
        Magpie::Player* player = new Player();
        
        //player->load_character_model();

        // Top-level transforms for each one of the animated models
        Scene::Transform *player_idle_trans = nullptr;
        Scene::Transform *player_walk_trans = nullptr;
        Scene::Transform *player_steal_trans = nullptr;

        // Tansforms for player animations
        std::vector< Scene::Transform* > player_model_walk_transforms;
        std::vector< Scene::Transform* > player_model_steal_transforms;
        std::vector< Scene::Transform* > player_model_idle_transforms;

        // Player Animation players
        TransformAnimationPlayer* magpie_walk_animation = nullptr;
        TransformAnimationPlayer* magpie_idle_animation = nullptr;
        TransformAnimationPlayer* magpie_steal_animation = nullptr;
    
        // Start constructing animations
        magpie_idle_animation = new TransformAnimationPlayer(*magpie_idle_tanim, player_model_idle_transforms, 1.0f, true);
        magpie_walk_animation = new TransformAnimationPlayer(*magpie_walk_tanim, player_model_walk_transforms, 1.0f, true);
        magpie_steal_animation = new TransformAnimationPlayer(*magpie_steal_tanim, player_model_steal_transforms, 1.0f, false);

        // Set animation states
        player->get_animation_manager()->add_state(new AnimationState(player_idle_trans, magpie_idle_animation));
        player->get_animation_manager()->add_state(new AnimationState(player_walk_trans, magpie_walk_animation));
        player->get_animation_manager()->add_state(new AnimationState(player_steal_trans, magpie_steal_animation));
        
        // Finally, set the transform for this player
        player->set_transform(player->get_animation_manager()->init(player->get_position(), (uint32_t)Player::STATE::IDLE)); 
        if (player->get_transform() == nullptr) {
            std::cerr << "ERROR:: Player Transform not found" << std::endl;
        }

        // Reposition as needed
        player->set_position(position);

        // Add the player to the game
        game.set_player(player);
    };

    void MagpieGameMode::create_guard(glm::vec3 position) {

        Magpie::Guard* guard = new Guard();

        //guard->load_character_model();

        // Use one main transform and swap it to point between
        // one of the three other specific transforms
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
        TransformAnimationPlayer* guard_idle_animation = nullptr;
        TransformAnimationPlayer* guard_patrol_animation = nullptr;
        TransformAnimationPlayer* guard_chase_animation = nullptr;
        TransformAnimationPlayer* guard_alert_animation = nullptr;
        TransformAnimationPlayer* guard_cautious_animation = nullptr;
        TransformAnimationPlayer* guard_confused_animation = nullptr;

        // Contructing Animations
        guard_patrol_animation = new TransformAnimationPlayer(*guard_dog_patrol_tanim, guard_model_patrol_transforms, 1.0f, true);
        guard_chase_animation = new TransformAnimationPlayer(*guard_dog_chase_tanim, guard_model_chase_transforms, 1.0f, true);
        guard_alert_animation = new TransformAnimationPlayer(*guard_dog_alert_tanim, guard_model_alert_transforms, 1.0f, false);
        guard_confused_animation = new TransformAnimationPlayer(*guard_dog_confused_tanim, guard_model_confused_transforms, 1.0f, true);
        guard_cautious_animation = new TransformAnimationPlayer(*guard_dog_cautious_tanim, guard_model_cautious_transforms, 1.0f, true);
        guard_idle_animation = new TransformAnimationPlayer(*guard_dog_idle_tanim, guard_model_idle_transforms, 1.0f, true);

        // Set animation states
        guard->get_animation_manager()->add_state(new AnimationState(guard_idle_trans, guard_idle_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_patrol_trans, guard_patrol_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_chase_trans, guard_chase_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_alert_trans, guard_alert_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_confused_trans, guard_confused_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_cautious_trans, guard_cautious_animation));

        // Finally, set the transform for this guard
        guard->set_transform(guard->get_animation_manager()->init(guard->get_position(), (uint32_t)Guard::STATE::IDLE)); 
        if (guard->get_transform() == nullptr) {
            std::cerr << "ERROR:: Guard Transform not found" << std::endl;
        }

        // Set the guard at the proper place
        guard->set_position(position);

        // Add the guard to the game
        game.add_guard(guard);
    };

    /**
     * Initializes the current level and positions the guards
     * and the player
     */
    void MagpieGameMode::load_level(std::string level_file) {

        Magpie::LevelLoader level_pixel_data;

        MagpieLevel* level = level_pixel_data.load(data_path(level_file), &scene, building_meshes.value, [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = vertex_color_program_info;
            default_program_info.vao = vertex_color_vaos->find("buildingTiles")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = building_meshes->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
            return obj;
        });

        game.set_level(level);

        // We are just using this for the camera positioning
        scene.load(data_path("levels/camera_transform.scene"), [&](Scene &s, Scene::Transform *t, std::string const &m){
            // Save resources
            scene.delete_transform(t);
        });

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
        
        // Move camera closer to the level
        camera_trans->position.x += 4.0f;
        camera_trans->position.y += 4.0f;

    };

    /**
     * Highlights the path the player will take by swapping the 
     * program attached to floor tiles with one that brightens
     * the colors in the fragment shader.
     */
    void MagpieGameMode::highlight_path_tiles() {
        //std::vector< FloorTile* >* highlighted_tiles = game.current_level->get_highlighted_tiles();
        //if (!highlighted_tiles->empty()) {
        //    for (uint32_t i = 0; i < highlighted_tiles->size(); i++) {
        //        (*highlighted_tiles)[i]->scene_object->programs[Scene::Object::ProgramTypeDefault] = vertex_color_program_info;
        //        (*highlighted_tiles)[i]->scene_object->programs[Scene::Object::ProgramTypeDefault].vao = *building_meshes_vao;
        //    }
        //}
        //highlighted_tiles->clear();
        //
        //std::vector<glm::vec2> path = game.get_player()->get_path();
        //FloorTile**** floor_matrix = game.current_level->get_floor_matrix();
        //for (uint32_t i = 0; i < path.size(); i++) {
        //    //(*floor_matrix)[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->active = false;
        //    //(*floor_matrix)[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->programs[Scene::Object::ProgramTypeDefault] = highlight_program_info;
        //    //(*floor_matrix)[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->programs[Scene::Object::ProgramTypeDefault].vao = *highlighted_building_meshes_vao;
        //    highlighted_tiles->push_back((*floor_matrix)[(uint32_t)path[i].x][(uint32_t)path[i].y]);
        //}
    }

    /**
     * Creates a ray projected from the camera, onto the world.
     * This is used for point-click mechanics
     */
    Magpie::Ray MagpieGameMode::create_click_ray(int mouseX, int mouseY, int screenWidth, int screenHeight, const Scene::Camera* cam) {
         glm::mat4 camLocalToWorld = cam->transform->make_local_to_world(); 

        float halfImageHeight = cam->near*std::tan(cam->fovy/2.0f);
        float halfImageWidth = cam->aspect*halfImageHeight;

        //3d Normalized device coords
        float normDeviceX = (2.0f * mouseX) / screenWidth - 1.0f;
        float normDeviceY = 1.0f - (2.0f * mouseY) / screenHeight;

        glm::vec3 localOrigin = glm::vec3(normDeviceX*halfImageWidth, normDeviceY*halfImageHeight, -cam->near);
        glm::vec3 worldOrigin = camLocalToWorld*glm::vec4(localOrigin, 1.0f);
        glm::vec3 worldDir = camLocalToWorld*glm::vec4(localOrigin, 0.0f);

        return Magpie::Ray(worldOrigin, worldDir);
    };

    /**
     * Calculates the point at which the given ray intersects with the horizontal
     * plane defined by the x and y axes.
     */
    glm::ivec3 MagpieGameMode::get_click_floor_intersect(Magpie::Ray click_ray, float floorHeight) {

        // float dist = worldOrigin.z - floorHeight;

        if (click_ray.direction.z>=0.0f) { //discard all rays going away from floor
            return glm::ivec3(-1,-1, 0);
        }
        //want to solve s.t. floorHeight = origin.z + t*worldDir.z
        float t = (floorHeight- click_ray.origin.z) / click_ray.direction.z;

        glm::vec3 pointOfIntersect = click_ray.origin + t * click_ray.direction;

        return pointOfIntersect;
    };

    /**
     * Returns true if the given ray intersected with any of the items
     * that are defined as clickable
     * 
     * WARNING:: This does not do depth checking in the case that one bounding
     * box is placed in-front of another
     */
    bool MagpieGameMode::handle_clickables(Magpie::Ray click_ray) {
        
        for (auto it = game.get_level()->get_paintings()->begin(); it != game.get_level()->get_paintings()->end(); it++) {
            for (auto paint_iter = it->second.begin(); paint_iter != it->second.end(); paint_iter++) {
                if (paint_iter->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)) {
                    paint_iter->steal(game.get_player());
                    paint_iter->on_click();
                    game.get_player()->set_state((uint32_t)Player::STATE::STEALING);
                    return true;
                }
            }
        }

        for (auto it = game.get_level()->get_gems()->begin(); it != game.get_level()->get_gems()->end(); it++) {
            for (auto gem_iter = it->second.begin(); gem_iter != it->second.end(); gem_iter++) {
                if (gem_iter->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)) {
                    gem_iter->steal(game.get_player());
                    gem_iter->on_click();
                    game.get_player()->set_state((uint32_t)Player::STATE::STEALING);
                    return true;
                }
            }
        }

        return false;
    };

    /**
     * Given the rounded position of where the ray intersected with the floor,
     * Sets the player's destination to be that position, and modifies the player's
     * state as needed.
     */
    bool MagpieGameMode::handle_player_movement(glm::ivec3 click_floor_intersect) {
        game.get_player()->setDestination(glm::ivec2(click_floor_intersect.x, click_floor_intersect.y));
        if (game.get_player()->get_state() == (uint32_t)Player::STATE::IDLE) {
            game.get_player()->set_state((uint32_t)Player::STATE::WALKING);
        }
        return true;
    };

    /**
     *  Geven a ray created by the player clicking on the view port,
     *  determines if the player clicked on a 'Clickable' object in the world
     *  or if they were clicking a space to move to
     */
    bool MagpieGameMode::handle_screen_click(Magpie::Ray click_ray) {
        bool handled = false;
        if (!handled) {
            handled =  handle_clickables(click_ray);
        }
        
        if (!handled) {
            glm::ivec3 point_of_intersect_at_floor = get_click_floor_intersect(click_ray, 0.0f);
            handled = handle_player_movement(point_of_intersect_at_floor);
        }
        return handled;
    };
    
}