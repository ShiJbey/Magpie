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

        create_player(glm::vec3(7.0f, 6.0f, 0.0f));
        game.get_player()->set_current_room(game.get_level()->get_tile_room_number(7.0f, 6.0f));

        create_guard(glm::vec3(7.0f, 8.0f, 0.0f));
        create_guard(glm::vec3(6.0f, 7.0f, 0.0f));
        create_guard(glm::vec3(8.0f, 7.0f, 0.0f));

        game.get_player()->set_state((uint32_t)Player::STATE::IDLE);
        game.get_guards()[1]->set_state((uint32_t)Guard::STATE::PATROLING);
        game.get_guards()[2]->set_state((uint32_t)Guard::STATE::CHASING);

        Navigation::getInstance().set_movement_matrix(game.get_level()->get_movement_matrix());
    };

    MagpieGameMode::~MagpieGameMode() {
        // Do Nothing
    };

    void MagpieGameMode::update(float elapsed) {

        // Update the player
        game.get_player()->update(elapsed);

        // Update the position of the guards
        for (uint32_t i = 0; i < game.get_guards().size(); i++) {
            game.get_guards()[i]->update(elapsed);
        }

        //camera_trans->position.x = game.get_player()->get_position().x;
        //camera_trans->position.y = game.get_player()->get_position().y;
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

        //keypress event handle
        if (evt.type == SDL_KEYDOWN && evt.key.repeat == 0) {
            if (evt.key.keysym.scancode == SDL_SCANCODE_M) { //M pressed for map
                ui.stateChanger('m');
                return true;
            }
            else if (evt.key.keysym.scancode == SDL_SCANCODE_I) { //I pressed for inventory
                ui.stateChanger('i');
                return true;
            }
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
    Scene::Transform* MagpieGameMode::load_character_model(GameCharacter* character, const ModelData* model_data, std::string model_name, std::string vao_key,
            Scene::Object::ProgramInfo program_info, const MeshBuffer* mesh_buffer) {

        Scene::Transform* model_group_transform = nullptr;

        model_group_transform = character->load_model(scene, model_data, model_name, [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info = program_info;
            default_program_info.vao = vertex_color_vaos->find(vao_key)->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = mesh_buffer->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
        });

        assert(model_group_transform != nullptr);

        return model_group_transform;
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
    std::vector< Scene::Transform* > MagpieGameMode::get_animation_transforms( std::unordered_map< std::string, Scene::Transform * >& name_to_transform, std::vector< std::string > names) {

        std::vector< Scene::Transform* > animation_transforms;

        for (auto const &name : names) {
            auto f = name_to_transform.find(name);
            if (f == name_to_transform.end()) {
                std::cerr << "WARNING: transform '" << name << "' appears in animation but not in scene." << std::endl;
                animation_transforms.emplace_back(nullptr);
            } else {
                animation_transforms.emplace_back(f->second);
            }
        }

        return animation_transforms;
    };


    Player* MagpieGameMode::create_player(glm::vec3 position) {

        Magpie::Player* player = new Player();

        // Top-level transforms for each one of the animated models
        Scene::Transform *player_idle_trans = load_character_model(player, magpie_idle_model.value, "Idle", "magpieIdle", vertex_color_program_info, magpie_idle_mesh.value);
        Scene::Transform *player_walk_trans = load_character_model(player, magpie_walk_model.value, "Walk", "magpieWalk", vertex_color_program_info, magpie_walk_mesh.value);
        Scene::Transform *player_steal_trans = load_character_model(player, magpie_steal_model.value, "Steal", "magpieSteal", vertex_color_program_info, magpie_steal_mesh.value);

        //look up various transforms for animations
        std::unordered_map< std::string, Scene::Transform * > name_to_transform;
        for (Scene::Transform *t = scene.first_transform; t != nullptr; t = t->alloc_next) {
            if (t->name.find("Magpie") != std::string::npos) {
                auto ret = name_to_transform.insert(std::make_pair(t->name, t));
                if (!ret.second) {
                    std::cerr << "WARNING: multiple transforms with the name '" << t->name << "' in scene." << std::endl;
                }
            }
        }

        // Tansforms for player animations
        std::vector< Scene::Transform* > player_model_idle_transforms = get_animation_transforms(name_to_transform, player->convert_animation_names(magpie_idle_tanim.value, "Idle"));
        std::vector< Scene::Transform* > player_model_walk_transforms = get_animation_transforms(name_to_transform, player->convert_animation_names(magpie_walk_tanim.value, "Walk"));
        std::vector< Scene::Transform* > player_model_steal_transforms = get_animation_transforms(name_to_transform, player->convert_animation_names(magpie_steal_tanim.value, "Steal"));

        // Start constructing animations
        TransformAnimationPlayer* magpie_idle_animation = new TransformAnimationPlayer(*magpie_idle_tanim, player_model_idle_transforms, 1.0f, true);
        TransformAnimationPlayer* magpie_walk_animation = new TransformAnimationPlayer(*magpie_walk_tanim, player_model_walk_transforms, 1.0f, true);
        TransformAnimationPlayer* magpie_steal_animation = new TransformAnimationPlayer(*magpie_steal_tanim, player_model_steal_transforms, 1.0f, false);

        // Set animation states
        player->get_animation_manager()->add_state(new AnimationState(player_idle_trans, magpie_idle_animation));
        player->get_animation_manager()->add_state(new AnimationState(player_walk_trans, magpie_walk_animation));
        player->get_animation_manager()->add_state(new AnimationState(player_steal_trans, magpie_steal_animation));

        // Finally, set the transform for this player
        player->set_transform(player->get_animation_manager()->init(position, (uint32_t)Player::STATE::IDLE));
        if (player->get_transform() == nullptr) {
            std::cerr << "ERROR:: Player Transform not found" << std::endl;
        }

        // Reposition as needed
        player->set_position(position);

        // Add the player to the game
        game.set_player(player);

        return player;
    };

    Guard* MagpieGameMode::create_guard(glm::vec3 position) {

        Magpie::Guard* guard = new Guard();

        // Use one main transform and swap it to point between
        // one of the three other specific transforms
        Scene::Transform *guard_idle_trans = load_character_model(guard, guard_dog_idle_model.value, "Idle", "guardIdle", vertex_color_program_info, guard_dog_idle_mesh.value);
        Scene::Transform *guard_patrol_trans = load_character_model(guard, guard_dog_patrol_model.value, "Patrol", "guardPatrol", vertex_color_program_info, guard_dog_patrol_mesh.value);
        Scene::Transform *guard_chase_trans = load_character_model(guard, guard_dog_chase_model.value, "Chase", "guardChase", vertex_color_program_info, guard_dog_chase_mesh.value);
        Scene::Transform *guard_alert_trans = load_character_model(guard, guard_dog_alert_model.value, "Alert", "guardAlert", vertex_color_program_info, guard_dog_alert_mesh.value);
        Scene::Transform *guard_confused_trans = load_character_model(guard, guard_dog_confused_model.value, "Confused", "guardConfused", vertex_color_program_info, guard_dog_confused_mesh.value);
        Scene::Transform *guard_cautious_trans = load_character_model(guard, guard_dog_cautious_model.value, "Cautious", "guardCautious", vertex_color_program_info, guard_dog_cautious_mesh.value);

        //look up various transforms for animations
        std::unordered_map< std::string, Scene::Transform * > name_to_transform;
        for (Scene::Transform *t = scene.first_transform; t != nullptr; t = t->alloc_next) {
            if (t->name.find("Guard") != std::string::npos) {
                auto ret = name_to_transform.insert(std::make_pair(t->name, t));
                if (!ret.second) {
                    std::cerr << "WARNING: multiple transforms with the name '" << t->name << "' in scene." << std::endl;
                }
            }
        }

        // Transforms for guard animations
        std::vector< Scene::Transform* > guard_model_idle_transforms = get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_idle_tanim.value, "Idle"));
        std::vector< Scene::Transform* > guard_model_patrol_transforms  = get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_patrol_tanim.value, "Patrol"));
        std::vector< Scene::Transform* > guard_model_chase_transforms  = get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_chase_tanim.value, "Chase"));
        std::vector< Scene::Transform* > guard_model_alert_transforms  = get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_alert_tanim.value, "Alert"));
        std::vector< Scene::Transform* > guard_model_confused_transforms  = get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_confused_tanim.value, "Confused"));
        std::vector< Scene::Transform* > guard_model_cautious_transforms  = get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_cautious_tanim.value, "Cautious"));

        // Contructing Animations
        TransformAnimationPlayer* guard_idle_animation = new TransformAnimationPlayer(*guard_dog_idle_tanim, guard_model_idle_transforms, 1.0f, true);
        TransformAnimationPlayer* guard_patrol_animation = new TransformAnimationPlayer(*guard_dog_patrol_tanim, guard_model_patrol_transforms, 1.0f, true);
        TransformAnimationPlayer* guard_chase_animation = new TransformAnimationPlayer(*guard_dog_chase_tanim, guard_model_chase_transforms, 1.0f, true);
        TransformAnimationPlayer* guard_alert_animation = new TransformAnimationPlayer(*guard_dog_alert_tanim, guard_model_alert_transforms, 1.0f, false);
        TransformAnimationPlayer* guard_confused_animation = new TransformAnimationPlayer(*guard_dog_confused_tanim, guard_model_confused_transforms, 1.0f, true);
        TransformAnimationPlayer* guard_cautious_animation = new TransformAnimationPlayer(*guard_dog_cautious_tanim, guard_model_cautious_transforms, 1.0f, true);


        // Set animation states
        guard->get_animation_manager()->add_state(new AnimationState(guard_idle_trans, guard_idle_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_patrol_trans, guard_patrol_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_chase_trans, guard_chase_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_alert_trans, guard_alert_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_confused_trans, guard_confused_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_cautious_trans, guard_cautious_animation));

        // Finally, set the transform for this guard
        guard->set_transform(guard->get_animation_manager()->init(position, (uint32_t)Guard::STATE::IDLE));
        if (guard->get_transform() == nullptr) {
            std::cerr << "ERROR:: Guard Transform not found" << std::endl;
        }

        // Set the guard at the proper place
        guard->set_position(position);

        // Add the guard to the game
        game.add_guard(guard);

        return guard;
    };

    /**
     * Initializes the current level and positions the guards
     * and the player
     */
    void MagpieGameMode::load_level(std::string level_file) {

        Magpie::LevelLoader level_pixel_data;

        MagpieLevel* level = level_pixel_data.load(data_path(level_file), &scene, building_meshes.value, [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info;

            if (m.find("wall") != std::string::npos) {
                default_program_info = transparent_program_info;
                default_program_info.vao = *highlighted_building_meshes_vao;
            } else {
                default_program_info = vertex_color_program_info;
                default_program_info.vao = vertex_color_vaos->find("buildingTiles")->second;
            }

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
        std::vector< FloorTile* >* highlighted_tiles = game.get_level()->get_highlighted_tiles();
        if (!highlighted_tiles->empty()) {
            for (uint32_t i = 0; i < highlighted_tiles->size(); i++) {
                (*highlighted_tiles)[i]->scene_object->programs[Scene::Object::ProgramTypeDefault] = vertex_color_program_info;
                (*highlighted_tiles)[i]->scene_object->programs[Scene::Object::ProgramTypeDefault].vao = *building_meshes_vao;
            }
        }
        highlighted_tiles->clear();

        std::vector<glm::vec2> path = game.get_player()->get_path()->get_path();
        FloorTile*** floor_matrix = game.get_level()->get_floor_matrix();
        for (uint32_t i = 0; i < path.size(); i++) {
            floor_matrix[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->programs[Scene::Object::ProgramTypeDefault] = highlight_program_info;
            floor_matrix[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->programs[Scene::Object::ProgramTypeDefault].vao = *highlighted_building_meshes_vao;
            highlighted_tiles->push_back(floor_matrix[(uint32_t)path[i].x][(uint32_t)path[i].y]);
        }
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
    glm::vec3 MagpieGameMode::get_click_floor_intersect(Magpie::Ray click_ray, float floorHeight) {

        // float dist = worldOrigin.z - floorHeight;

        if (click_ray.direction.z>=0.0f) { //discard all rays going away from floor
            return glm::ivec3(-1,-1, 0);
        }
        //want to solve s.t. floorHeight = origin.z + t*worldDir.z
        float t = (floorHeight- click_ray.origin.z) / click_ray.direction.z;

        glm::vec3 pointOfIntersect = click_ray.origin + t * click_ray.direction;

        // Rounds the intersection to be a whole number tile position
        pointOfIntersect.x = floor(pointOfIntersect.x + 0.5f);
        pointOfIntersect.y = floor(pointOfIntersect.y + 0.5f);

        printf("Player clicked tile: (%f, %f, %f)\n", pointOfIntersect.x, pointOfIntersect.y, pointOfIntersect.z);

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
                if (paint_iter->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)
                    && paint_iter->get_scene_object()->active) {
                    paint_iter->steal(game.get_player());
                    paint_iter->on_click();
                    game.get_player()->set_state((uint32_t)Player::STATE::STEALING);
                    return true;
                }
            }
        }

        for (auto it = game.get_level()->get_gems()->begin(); it != game.get_level()->get_gems()->end(); it++) {
            for (auto gem_iter = it->second.begin(); gem_iter != it->second.end(); gem_iter++) {
                if (gem_iter->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)
                 && gem_iter->get_scene_object()->active) {
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
    bool MagpieGameMode::handle_player_movement(glm::vec3 click_floor_intersect) {
        if (game.get_level()->can_move_to(game.get_player()->get_current_room(), click_floor_intersect.x, click_floor_intersect.y)) {
            game.get_player()->set_path(Magpie::Navigation::getInstance().findPath(
                glm::vec2(game.get_player()->get_position().x, game.get_player()->get_position().y),
                glm::vec2(click_floor_intersect.x, click_floor_intersect.y)));
            //highlight_path_tiles();

            std::cout << "DEBUG:: Player can move to that position" << std::endl;
            glm::vec2 destination = game.get_player()->get_path()->next();

            game.get_player()->setDestination(glm::vec3(destination.x, destination.y, 0.0f));

            if (game.get_player()->get_state() == (uint32_t)Player::STATE::IDLE) {
                game.get_player()->set_state((uint32_t)Player::STATE::WALKING);
            }
            return true;
        } else {
            std::cout << "DEBUG:: Player can't move to that position" << std::endl;
        }
        return false;
    };

    /**
     *  Geven a ray created by the player clicking on the view port,
     *  determines if the player clicked on a 'Clickable' object in the world
     *  or if they were clicking a space to move to
     */
    bool MagpieGameMode::handle_screen_click(Magpie::Ray click_ray) {
        bool handled = false;
        if (!handled) {
            //handled =  handle_clickables(click_ray);
        }

        if (!handled) {
            handled = handle_player_movement(get_click_floor_intersect(click_ray, 0.0f));
        }
        return handled;
    };

}