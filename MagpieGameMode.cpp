#include "MagpieGamemode.hpp"

#include "AssetLoader.hpp"
#include "Clickable.hpp"
#include "AnimationManager.hpp"
#include "TransformAnimation.hpp"
#include "load_level.hpp"
#include "MagpieGame.hpp"
#include "draw_freetype_text.hpp"
#include "AnimatedModel.hpp"
#include "startmenu.hpp"

#include "TutorialMode.hpp"
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
#include <deque>
#include <tuple>
#include <algorithm>
#include <random>

#define FREE_FLIGHT // Enables the user to move the camera using the arrow keys
#define GUARD_DEBUG // allows you to click on guards to get debug information

namespace Magpie {

    MagpieGameMode::MagpieGameMode() {

        // build a level from the given level data
        load_level(final_map.value);
        assert(game.get_level() != nullptr);

        // Obtain camera positioning from blender scene
        setup_camera();
        
        // Set up the player
        glm::vec3 player_position = game.get_level()->get_player_start_position();
        create_player(player_position);
        game.get_player()->set_current_room(game.get_level()->get_tile_room_number(player_position.x, player_position.y));
        game.get_player()->set_state((uint32_t)Player::STATE::IDLE);

        // Instantiate Guards
        auto guard_start = game.get_level()->get_guard_start_positions();
        for (auto i : guard_start) {
            for (auto i2 : i.second) {
               Guard* guard = create_guard(i2.second.first, i2.second.second);
               auto path = game.get_level()->get_guard_path(i.first, i2.first);
               guard->set_patrol_points(path);
               guards.push_back(guard);
               guard->dog_treats_in_level = &dog_treats;
            }
        }

        // Set up the navigator
        Navigation::getInstance().set_movement_matrix(game.get_level()->get_movement_matrix());

        make_close_walls_transparent(game.get_player()->get_position().x, game.get_player()->get_position().y);

        game.current_music = sample_ambient->play(game.get_player()->get_position(), 0.3f, Sound::Loop);
    };

    MagpieGameMode::~MagpieGameMode() {
        // Do Nothing
    };

    void MagpieGameMode::update(float elapsed) {
        if (game.get_player()->game_over) {
            Mode::set_current(std::make_shared< Magpie::EndMenu >());
            return;
        }
        //if the map is out don't update anything
        if (ui.map.state == Map::OFF) {
            // Update the player
            game.get_player()->update(elapsed);

            // Update the guards
            for (uint32_t i = 0; i < game.get_guards().size(); i++) {
                game.get_guards()[i]->update(elapsed);
            }

            // Update any objects in the scene running animations
            for (uint32_t i = 0; i < animated_scene_objects.size(); i++) {
                animated_scene_objects[i]->get_animation_manager()->update(elapsed);
            }

            for (auto const &room: game.get_level()->get_gems()) {
                for (auto &gem: room.second) {
                    gem->update_animation(elapsed);
                }
            }

            for (auto const &room: game.get_level()->get_paintings()) {
                for (auto &painting: room.second) {
                    painting->update_animation(elapsed);
                }
            }

            if(game.get_level()->pink_card != nullptr && !game.get_player()->has_pink_card) {
                game.get_level()->pink_card->update_animation(elapsed);
            }

            if(game.get_level()->green_card != nullptr && !game.get_player()->has_green_card) {
                game.get_level()->green_card->update_animation(elapsed);
            }

            if(game.get_level()->master_key != nullptr && !game.get_player()->has_master_key) {
                game.get_level()->master_key->update_animation(elapsed);
            }

            if(game.get_level()->dogTreatPickUp != nullptr && !game.get_player()->has_dog_treats) {
                game.get_level()->dogTreatPickUp->update_animation(elapsed);
            }

            if(game.get_level()->cardboard_box != nullptr && !game.get_player()->has_cardboard_box) {
                game.get_level()->cardboard_box->update_animation(elapsed);
            }

            // update animated text
            for(auto i = animated_text_objects.begin(); i != animated_text_objects.end(); /*Incremement in loop*/) {
                if (i->animation_complete()) {
                    i = animated_text_objects.erase(i);
                }
                else {
                    i->update_animation(elapsed);
                    i++;
                }
            }

            //update inventory too since map is off
            ui.inventory.updateInv(elapsed);

            //update background music location
            game.current_music->set_position(game.get_player()->get_position());
        
            #ifndef FREE_FLIGHT
            camera_trans->position.x = game.get_player()->get_position().x;
            camera_trans->position.y = game.get_player()->get_position().y;
            #endif
        }
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
            else if (evt.key.keysym.scancode == SDL_SCANCODE_SPACE && game.get_player()->has_dog_treats) {
                if (game.get_player()->can_place_treat()) {
                    dog_treats.push_back(drop_treat(game.get_player()->get_position())->get_scene_object());
                    game.get_player()->reset_treat_cooldown();
                }
                else {
                    animated_text_objects.push_back(FloatingNotificationText("Making more treats...", ransom_font.value, glm::vec2(screen_dimensions.x / 2.0f - 30.0f, screen_dimensions.y / 2.0f + 30.0f), 0.75f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f));
                }
            }
            else if (evt.key.keysym.scancode == SDL_SCANCODE_D && game.get_player()->has_cardboard_box) {
                //printf("Swapping disguise\n");
                switch(game.get_player()->get_state()) {
                    case (uint32_t)Player::STATE::IDLE:
                        sample_magpie_disguise->play(game.get_player()->get_position());
                        game.get_player()->set_state((uint32_t)Player::STATE::DISGUISE_IDLE);
                        break;
                    case (uint32_t)Player::STATE::WALKING:
                        sample_magpie_disguise->play(game.get_player()->get_position());
                        game.get_player()->set_state((uint32_t)Player::STATE::DISGUISE_WALK);
                        break;
                    case (uint32_t)Player::STATE::DISGUISE_IDLE:
                        game.get_player()->set_state((uint32_t)Player::STATE::IDLE);
                        break;
                    case (uint32_t)Player::STATE::DISGUISE_WALK:
                        game.get_player()->set_state((uint32_t)Player::STATE::WALKING);
                        break;
                }
            }
            else if (evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                //open tutorial screen on 'ESCAPE':
                show_tutorial();
                return true;
            }
            else if (evt.key.keysym.scancode == SDL_SCANCODE_EQUALS) {
                presses_for_developer_mode--;
                if (presses_for_developer_mode == 0) {
                    game.get_player()->has_pink_card = true;
                    game.get_player()->has_green_card= true;
                    game.get_player()->has_master_key = true;
                    game.get_player()->has_cardboard_box = true;
                    game.get_player()->has_dog_treats = true;
                    animated_text_objects.push_back(FloatingNotificationText("Cheat Active!", ransom_font.value, glm::vec2(screen_dimensions.x / 2.0f - 30.0f, screen_dimensions.y / 2.0f + 30.0f), 0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f));
                }
            }
            #ifdef GUARD_DEBUG
            // Reset all the guards to not print output
            else if(evt.key.keysym.scancode == SDL_SCANCODE_1) {
                for (auto &guard : guards) {
                    guard->debug_focus = false;
                }
            }
            #endif
        }

        #ifdef FREE_FLIGHT
        if (evt.type == SDL_KEYDOWN) {
            switch(evt.key.keysym.scancode) {
                case SDL_SCANCODE_LEFT:
                    camera_trans->position.x -= 1.0f;
                    break;
                case SDL_SCANCODE_RIGHT:
                    camera_trans->position.x += 1.0f;
                    break;
                case SDL_SCANCODE_UP:
                    camera_trans->position.y += 1.0f;
                    break;
                case SDL_SCANCODE_DOWN:
                    camera_trans->position.y -= 1.0f;
                    break;
                default:
                    break;
            }
        }
        #endif

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
            glUniform3fv(vertex_color_program->sky_color_vec3, 1, glm::value_ptr(glm::vec3(0.9f, 0.9f, 0.9f)));
            glUniform3fv(vertex_color_program->sky_direction_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
            camera->aspect = drawable_size.x / float(drawable_size.y);
            //Draw scene:
            scene.draw(camera);
        }

        {
            //score
            if (Mode::current == shared_from_this()) {
                RenderText(ransom_font.value, "$" + std::to_string(game.get_player()->get_score()),
                           0.0f, 0.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
                RenderText(tutorial_font.value, "'Click' to move and select, press 'ESC' for instructions",
                           (float)drawable_size.x - 550.0f, 20.0f, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
                //draw UI
                ui.drawUI(camera, drawable_size);
            }
        }

        {
            // draw animated text
            for (auto &text : animated_text_objects) {
                text.draw();
            }
        }

        GL_ERRORS();

        screen_dimensions = glm::vec2((float)drawable_size.x, (float)drawable_size.y);
    };

    
    Player* MagpieGameMode::create_player(glm::vec3 position) {

        Magpie::Player* player = new Player();

        // Top-level transforms for each one of the animated models
        Scene::Transform *player_idle_trans = LevelLoader::load_animated_model(scene, *player, magpie_idle_model.value, "Idle", "magpieIdle", *vertex_color_program_info.value, magpie_idle_mesh.value);
        Scene::Transform *player_walk_trans = LevelLoader::load_animated_model(scene, *player, magpie_walk_model.value, "Walk", "magpieWalk", *vertex_color_program_info.value, magpie_walk_mesh.value);
        Scene::Transform *player_steal_trans = LevelLoader::load_animated_model(scene, *player, magpie_steal_model.value, "Steal", "magpieSteal", *vertex_color_program_info.value, magpie_steal_mesh.value);
        Scene::Transform *player_pick_trans = LevelLoader::load_animated_model(scene, *player, magpie_pick_model.value, "Pick", "magpiePick", *vertex_color_program_info.value, magpie_pick_mesh.value);
        Scene::Transform *player_break_trans = LevelLoader::load_animated_model(scene, *player, magpie_break_model.value, "Break", "magpieBreak", *vertex_color_program_info.value, magpie_break_mesh.value);
        Scene::Transform *player_disguise_idle_trans = LevelLoader::load_animated_model(scene, *player, magpie_disguise_idle_model.value, "DisguiseIdle", "magpieDisguiseIdle", *vertex_color_program_info.value, magpie_disguise_idle_mesh.value);
        Scene::Transform *player_disguise_walk_trans = LevelLoader::load_animated_model(scene, *player, magpie_disguise_walk_model.value, "DisguiseWalk", "magpieDisguiseWalk", *vertex_color_program_info.value, magpie_disguise_walk_mesh.value);

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
        std::vector< Scene::Transform* > player_model_idle_transforms = LevelLoader::get_animation_transforms(name_to_transform, player->convert_animation_names(magpie_idle_tanim.value, "Idle"));
        std::vector< Scene::Transform* > player_model_walk_transforms = LevelLoader::get_animation_transforms(name_to_transform, player->convert_animation_names(magpie_walk_tanim.value, "Walk"));
        std::vector< Scene::Transform* > player_model_steal_transforms = LevelLoader::get_animation_transforms(name_to_transform, player->convert_animation_names(magpie_steal_tanim.value, "Steal"));
        std::vector< Scene::Transform* > player_model_pick_transforms = LevelLoader::get_animation_transforms(name_to_transform, player->convert_animation_names(magpie_pick_tanim.value, "Pick"));
        std::vector< Scene::Transform* > player_model_break_transforms = LevelLoader::get_animation_transforms(name_to_transform, player->convert_animation_names(magpie_break_tanim.value, "Break"));
        std::vector< Scene::Transform* > player_model_disguise_idle_transforms = LevelLoader::get_animation_transforms(name_to_transform, player->convert_animation_names(magpie_disguise_idle_tanim.value, "DisguiseIdle"));
        std::vector< Scene::Transform* > player_model_disguise_walk_transforms = LevelLoader::get_animation_transforms(name_to_transform, player->convert_animation_names(magpie_disguise_walk_tanim.value, "DisguiseWalk"));

        // Start constructing animations
        TransformAnimationPlayer* magpie_idle_animation = new TransformAnimationPlayer(*magpie_idle_tanim, player_model_idle_transforms, 1.0f, true);
        TransformAnimationPlayer* magpie_walk_animation = new TransformAnimationPlayer(*magpie_walk_tanim, player_model_walk_transforms, 1.0f, true);
        TransformAnimationPlayer* magpie_steal_animation = new TransformAnimationPlayer(*magpie_steal_tanim, player_model_steal_transforms, 1.0f, false);
        TransformAnimationPlayer* magpie_pick_animation = new TransformAnimationPlayer(*magpie_pick_tanim, player_model_pick_transforms, 1.0f, false);
        TransformAnimationPlayer* magpie_break_animation = new TransformAnimationPlayer(*magpie_break_tanim, player_model_break_transforms, 1.0f, false);
        TransformAnimationPlayer* magpie_disguise_idle_animation = new TransformAnimationPlayer(*magpie_disguise_idle_tanim, player_model_disguise_idle_transforms, 1.0f, true);
        TransformAnimationPlayer* magpie_disguise_walk_animation = new TransformAnimationPlayer(*magpie_disguise_walk_tanim, player_model_disguise_walk_transforms, 1.0f, true);

        // Set animation states
        player->get_animation_manager()->add_state(new AnimationState(player_idle_trans, magpie_idle_animation));
        player->get_animation_manager()->add_state(new AnimationState(player_walk_trans, magpie_walk_animation));
        player->get_animation_manager()->add_state(new AnimationState(player_steal_trans, magpie_steal_animation));
        player->get_animation_manager()->add_state(new AnimationState(player_pick_trans, magpie_pick_animation));
        player->get_animation_manager()->add_state(new AnimationState(player_break_trans, magpie_break_animation));
        player->get_animation_manager()->add_state(new AnimationState(player_disguise_idle_trans, magpie_disguise_idle_animation));
        player->get_animation_manager()->add_state(new AnimationState(player_disguise_walk_trans, magpie_disguise_walk_animation));

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

    Guard* MagpieGameMode::create_guard(glm::vec3 position, GameAgent::DIRECTION dir) {

        Magpie::Guard* guard = new Guard();
        guard->player = game.get_player();

        // Use one main transform and swap it to point between
        // one of the three other specific transforms
        Scene::Transform *guard_idle_trans = LevelLoader::load_animated_model(scene, *guard, guard_dog_idle_model.value, "Idle", "guardIdle", *vertex_color_program_info.value, guard_dog_idle_mesh.value);
        Scene::Transform *guard_patrol_trans = LevelLoader::load_animated_model(scene, *guard, guard_dog_patrol_model.value, "Patrol", "guardPatrol", *vertex_color_program_info.value, guard_dog_patrol_mesh.value);
        Scene::Transform *guard_chase_trans = LevelLoader::load_animated_model(scene, *guard, guard_dog_chase_model.value, "Chase", "guardChase", *vertex_color_program_info.value, guard_dog_chase_mesh.value);
        Scene::Transform *guard_alert_trans = LevelLoader::load_animated_model(scene, *guard, guard_dog_alert_model.value, "Alert", "guardAlert", *vertex_color_program_info.value, guard_dog_alert_mesh.value);
        Scene::Transform *guard_confused_trans = LevelLoader::load_animated_model(scene, *guard, guard_dog_confused_model.value, "Confused", "guardConfused", *vertex_color_program_info.value, guard_dog_confused_mesh.value);
        Scene::Transform *guard_cautious_trans = LevelLoader::load_animated_model(scene, *guard, guard_dog_cautious_model.value, "Cautious", "guardCautious", *vertex_color_program_info.value, guard_dog_cautious_mesh.value);
        Scene::Transform *guard_eat_trans = LevelLoader::load_animated_model(scene, *guard, guard_dog_eat_model.value, "Eat", "guardEat", *vertex_color_program_info.value, guard_dog_eat_mesh.value);

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
        std::vector< Scene::Transform* > guard_model_idle_transforms = LevelLoader::get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_idle_tanim.value, "Idle"));
        std::vector< Scene::Transform* > guard_model_patrol_transforms  = LevelLoader::get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_patrol_tanim.value, "Patrol"));
        std::vector< Scene::Transform* > guard_model_chase_transforms  = LevelLoader::get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_chase_tanim.value, "Chase"));
        std::vector< Scene::Transform* > guard_model_alert_transforms  = LevelLoader::get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_alert_tanim.value, "Alert"));
        std::vector< Scene::Transform* > guard_model_confused_transforms  = LevelLoader::get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_confused_tanim.value, "Confused"));
        std::vector< Scene::Transform* > guard_model_cautious_transforms  = LevelLoader::get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_cautious_tanim.value, "Cautious"));
        std::vector< Scene::Transform* > guard_model_eat_transforms  = LevelLoader::get_animation_transforms(name_to_transform, guard->convert_animation_names(guard_dog_eat_tanim.value, "Eat"));

        // Contructing Animations
        TransformAnimationPlayer* guard_idle_animation = new TransformAnimationPlayer(*guard_dog_idle_tanim, guard_model_idle_transforms, 1.0f, true);
        TransformAnimationPlayer* guard_patrol_animation = new TransformAnimationPlayer(*guard_dog_patrol_tanim, guard_model_patrol_transforms, 1.0f, true);
        TransformAnimationPlayer* guard_chase_animation = new TransformAnimationPlayer(*guard_dog_chase_tanim, guard_model_chase_transforms, 1.0f, true);
        TransformAnimationPlayer* guard_alert_animation = new TransformAnimationPlayer(*guard_dog_alert_tanim, guard_model_alert_transforms, 1.0f, false);
        TransformAnimationPlayer* guard_confused_animation = new TransformAnimationPlayer(*guard_dog_confused_tanim, guard_model_confused_transforms, 1.0f, true);
        TransformAnimationPlayer* guard_cautious_animation = new TransformAnimationPlayer(*guard_dog_cautious_tanim, guard_model_cautious_transforms, 1.0f, true);
        TransformAnimationPlayer* guard_eat_animation = new TransformAnimationPlayer(*guard_dog_eat_tanim, guard_model_eat_transforms, 1.0f, false);

        // Set animation states
        guard->get_animation_manager()->add_state(new AnimationState(guard_idle_trans, guard_idle_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_patrol_trans, guard_patrol_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_chase_trans, guard_chase_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_alert_trans, guard_alert_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_confused_trans, guard_confused_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_cautious_trans, guard_cautious_animation));
        guard->get_animation_manager()->add_state(new AnimationState(guard_eat_trans, guard_eat_animation));

        // Finally, set the transform for this guard
        guard->set_transform(guard->get_animation_manager()->init(position, (uint32_t)Guard::STATE::IDLE));
        if (guard->get_transform() == nullptr) {
            std::cerr << "ERROR:: Guard Transform not found" << std::endl;
        }

        glm::vec3 turn_destination = position;
        switch (dir) {
            case GameAgent::DIRECTION::LEFT:
                turn_destination.x--;
                break;
            case GameAgent::DIRECTION::RIGHT:
                turn_destination.x++;
                break;
            case GameAgent::DIRECTION::UP:
                turn_destination.y++;
                break;
            default:
                turn_destination.y--;
                break;
        }

        // Set the guard at the proper place
        guard->set_position(position);
        guard->set_starting_point(position);
        guard->turnTo(turn_destination);
        guard->set_model_orientation((uint32_t)dir);

        assert(guard->get_orientation() == dir);

        // Add the guard to the game
        game.add_guard(guard);

        return guard;
    };

    DogTreat* MagpieGameMode::drop_treat(glm::vec3 position) {
        Scene::Transform* temp_transform = scene.new_transform();
        temp_transform->position = position;

        Scene::Object *obj = scene.new_object(temp_transform);
        Scene::Object::ProgramInfo default_program_info;
        default_program_info = *vertex_color_program_info.value;
        default_program_info.vao = vertex_color_vaos->find("buildingTiles")->second;
        obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
        MeshBuffer::Mesh const &mesh = building_meshes->lookup("dogTreat_MSH");
        obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
        obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;

        DogTreat* dog_treat = new DogTreat(obj);
        obj->transform->rotation *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

        return dog_treat;
    };

    /**
     * Initializes the current level and positions the guards
     * and the player
     */
    void MagpieGameMode::load_level(const LevelData *level_data) {

        MagpieLevel *level = LevelLoader::load(level_data, scene, building_meshes.value, [&](Scene &s, Scene::Transform *t, std::string const &m){
            Scene::Object *obj = s.new_object(t);
            Scene::Object::ProgramInfo default_program_info;
            default_program_info = *vertex_color_program_info.value;
            default_program_info.vao = vertex_color_vaos->find("buildingTiles")->second;
            obj->programs[Scene::Object::ProgramTypeDefault] = default_program_info;
            MeshBuffer::Mesh const &mesh = building_meshes->lookup(m);
            obj->programs[Scene::Object::ProgramTypeDefault].start = mesh.start;
            obj->programs[Scene::Object::ProgramTypeDefault].count = mesh.count;
            return obj;
        });

        game.set_level(level);
    };

    void MagpieGameMode::setup_camera() {
        // We are just using this for the camera positioning
        scene.load(data_path("levels/camera_transform.scene"), [](Scene &s, Scene::Transform *t, std::string const &m){
            // Save resources
            s.delete_transform(t);
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
    }

    /**
     * Highlights the path the player will take by swapping the
     * program attached to floor tiles with one that brightens
     * the colors in the fragment shader.
     */
    void MagpieGameMode::highlight_path_tiles() {
        if (!highlighted_tiles.empty()) {
            for (uint32_t i = 0; i < highlighted_tiles.size(); i++) {
                Scene::Object::ProgramInfo old_info = highlighted_tiles[i]->scene_object->programs[Scene::Object::ProgramTypeDefault];
                highlighted_tiles[i]->scene_object->programs[Scene::Object::ProgramTypeDefault] = *vertex_color_program_info.value;
                highlighted_tiles[i]->scene_object->programs[Scene::Object::ProgramTypeDefault].vao = *building_meshes_vao;
                highlighted_tiles[i]->scene_object->programs[Scene::Object::ProgramTypeDefault].start = old_info.start;
                highlighted_tiles[i]->scene_object->programs[Scene::Object::ProgramTypeDefault].count = old_info.count;
            }
        }
        highlighted_tiles.clear();

        std::vector<glm::vec2> path = game.get_player()->get_path()->get_path();
        FloorTile*** floor_matrix = game.get_level()->get_floor_matrix();
        for (uint32_t i = 0; i < path.size(); i++) {
            Scene::Object::ProgramInfo old_info = floor_matrix[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->programs[Scene::Object::ProgramTypeDefault];
            floor_matrix[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->programs[Scene::Object::ProgramTypeDefault] = *highlight_program_info.value;
            floor_matrix[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->programs[Scene::Object::ProgramTypeDefault].vao = *highlighted_building_meshes_vao;
            highlighted_tiles.push_back(floor_matrix[(uint32_t)path[i].x][(uint32_t)path[i].y]);
            floor_matrix[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->programs[Scene::Object::ProgramTypeDefault].start = old_info.start;
            floor_matrix[(uint32_t)path[i].x][(uint32_t)path[i].y]->scene_object->programs[Scene::Object::ProgramTypeDefault].count = old_info.count;
        }
    };

    /**
     * Performs a BFS on the walls in the player's current_room
     * and makes all room walls to the left and below the character
     * transparent
     */
    void Magpie::MagpieGameMode::make_close_walls_transparent(float x, float y) {

        for (uint32_t i = 0; i < transparent_walls.size(); i++ ) {
            Scene::Object::ProgramInfo old_info = transparent_walls[i]->scene_object->programs[Scene::Object::ProgramTypeDefault];
            transparent_walls[i]->scene_object->programs[Scene::Object::ProgramTypeDefault] = *vertex_color_program_info.value;
            transparent_walls[i]->scene_object->programs[Scene::Object::ProgramTypeDefault].vao = *building_meshes_vao;
            transparent_walls[i]->scene_object->programs[Scene::Object::ProgramTypeDefault].start = old_info.start;
            transparent_walls[i]->scene_object->programs[Scene::Object::ProgramTypeDefault].count = old_info.count;
        }
        transparent_walls.clear();

        float player_pos_x = x;
        float player_pos_y = y;

        std::vector< glm::vec2 > visited;
        // Unexplored grid positions
        std::deque< glm::vec2 > frontier;
        frontier.emplace_back(glm::uvec2(player_pos_x, player_pos_y));
        visited.push_back(glm::uvec2(player_pos_x, player_pos_y));

        while (!frontier.empty()) {

            glm::vec2 current = frontier.front();
            frontier.pop_front();

            std::vector< glm::vec2 > adjacent_tiles;
            adjacent_tiles.emplace_back(current.x + 1, current.y);
            adjacent_tiles.emplace_back(current.x - 1, current.y);
            adjacent_tiles.emplace_back(current.x, current.y + 1);
            adjacent_tiles.emplace_back(current.x, current.y - 1);

            for (auto &pos: adjacent_tiles) {
                 // Check the tile above this one
                if (game.get_level()->is_wall(pos.x, pos.y)) {
                    // check if the position has been visited
                    if(std::find(visited.begin(), visited.end(), pos) == visited.end()) {
                        // Swap out the program information
                        if (game.get_level()->get_wall(pos.x, pos.y)->room_number != game.get_level()->get_tile_room_number(x, y)) {
                            Wall* wall = game.get_level()->get_wall(pos.x, pos.y);
                            assert(wall != nullptr);
                            Scene::Object::ProgramInfo old_info = wall->scene_object->programs[Scene::Object::ProgramTypeDefault];
                            wall->scene_object->programs[Scene::Object::ProgramTypeDefault] = *transparent_program_info.value;
                            wall->scene_object->programs[Scene::Object::ProgramTypeDefault].vao = *transparent_building_meshes_vao;
                            wall->scene_object->programs[Scene::Object::ProgramTypeDefault].start = old_info.start;
                            wall->scene_object->programs[Scene::Object::ProgramTypeDefault].count = old_info.count;
                            wall->scene_object->programs[Scene::Object::ProgramTypeDefault].program = 0;
                            visited.push_back(pos);
                            transparent_walls.push_back(wall);
                        }
                    }
                }
                else if (game.get_level()->can_move_to(game.get_player()->get_current_room(), pos.x, pos.y) && 
                    game.get_level()->get_tile_room_number(pos.x, pos.y) == game.get_level()->get_tile_room_number(x, y)) {
                    
                    if(std::find(visited.begin(), visited.end(), pos) == visited.end()) {
                        // Add this position to the frontier
                        visited.push_back(pos);
                        frontier.push_back(pos);
                    }
                }
            }
        }

    };

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
        worldDir = glm::normalize(worldDir);

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

        auto play_score_sound = [&](uint32_t score_value) {
            if (score_value < 10000) {
                sample_point2->play(game.get_player()->get_position(), 0.2f);
            } else if (score_value < 100000) {
                sample_point1->play(game.get_player()->get_position(), 0.2f);
            } else {
                sample_point3->play(game.get_player()->get_position(), 0.2f);
            }
        };

        for (const auto &room: game.get_level()->get_paintings()) {
            if(room.first != game.get_player()->get_current_room()) continue;
            for (auto &painting: room.second) {
                if (painting->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)
                    && painting->get_scene_object()->active
                    && abs(game.get_player()->get_position().x - painting->get_position().x) <= 1
                    && abs(game.get_player()->get_position().y - painting->get_position().y) <= 1) {
                    painting->steal(game.get_player()); //changing player score
                    painting->on_click();
                    game.get_player()->set_state((uint32_t)Player::STATE::STEALING);
                    animated_text_objects.push_back(FloatingNotificationText("+$" + std::to_string(painting->get_selling_price()), ransom_font.value, glm::vec2(screen_dimensions.x / 2.0f - 30.0f, screen_dimensions.y / 2.0f + 30.0f), 0.75f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f));
                    sample_steal1->play(game.get_player()->get_position());
                    play_score_sound(painting->get_selling_price());
                    return true;
                }
            }
        }

        for (const auto &room: game.get_level()->get_gems()) {
            if(room.first != game.get_player()->get_current_room()) continue;
            for (auto &gem: room.second) {
                if (gem->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)
                    && gem->get_scene_object()->active
                    && abs(game.get_player()->get_position().x - gem->get_position().x) <= 1
                    && abs(game.get_player()->get_position().y - gem->get_position().y) <= 1) {
                    gem->steal(game.get_player());
                    gem->on_click();
                    game.get_player()->set_state((uint32_t)Player::STATE::STEALING);
                    animated_text_objects.push_back(FloatingNotificationText("+$" + std::to_string(gem->get_selling_price()), ransom_font.value, glm::vec2(screen_dimensions.x / 2.0f - 30.0f, screen_dimensions.y / 2.0f + 30.0f), 0.75f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f));
                    sample_steal1->play(game.get_player()->get_position());
                    play_score_sound(gem->get_selling_price());
                    return true;
                }
            }
        }

        for (auto const &displaycase : game.get_level()->get_displaycases()) {
            if(displaycase->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)
               && abs(game.get_player()->get_position().x - displaycase->get_position().x) <= 1
               && abs(game.get_player()->get_position().y - displaycase->get_position().y) <= 1) {
                if (!displaycase->opened) {
                    displaycase->on_click();
                    sample_unlock1->play(game.get_player()->get_position());
                    return true;
                }
                else if (displaycase->opened && displaycase->geode != nullptr && displaycase->geode->get_scene_object()->active) {
                    displaycase->geode->steal(game.get_player());
                    displaycase->geode->get_scene_object()->active = false;
                    animated_text_objects.push_back(FloatingNotificationText("+$" + std::to_string(displaycase->geode->get_selling_price()), ransom_font.value, glm::vec2(screen_dimensions.x / 2.0f - 30.0f, screen_dimensions.y / 2.0f + 30.0f), 0.75f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f));
                    sample_steal1->play(game.get_player()->get_position());
                    play_score_sound(displaycase->geode->get_selling_price());
                    return true;
                }
            }
        }

        if(game.get_level()->pink_card != nullptr && !game.get_player()->has_pink_card) {
            if (game.get_level()->pink_card->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)
                && game.get_level()->pink_card->get_scene_object()->active
                && abs(game.get_player()->get_position().x - game.get_level()->pink_card->get_position().x) <= 1
                && abs(game.get_player()->get_position().y - game.get_level()->pink_card->get_position().y) <= 1) {
                game.get_level()->pink_card->on_click();
                game.get_player()->has_pink_card = true;
                animated_text_objects.push_back(FloatingNotificationText("Found Pink Keycard", tutorial_font.value, glm::vec2(screen_dimensions.x / 2.0f - 50.0f, screen_dimensions.y / 2.0f + 50.0f), 0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 3.0f));
                sample_pickup->play(game.get_player()->get_position());
                return true;
            }
        }

        if(game.get_level()->green_card != nullptr && !game.get_player()->has_green_card) {
            if (game.get_level()->green_card->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)
                && game.get_level()->green_card->get_scene_object()->active
                && abs(game.get_player()->get_position().x - game.get_level()->green_card->get_position().x) <= 1
                && abs(game.get_player()->get_position().y - game.get_level()->green_card->get_position().y) <= 1) {
                game.get_level()->green_card->on_click();
                game.get_player()->has_green_card = true;
                animated_text_objects.push_back(FloatingNotificationText("Found Green Keycard", tutorial_font.value, glm::vec2(screen_dimensions.x / 2.0f - 100.0f, screen_dimensions.y / 2.0f + 50.0f), 0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 3.0f));
                sample_pickup->play(game.get_player()->get_position());
                return true;
            }
        }

        if(game.get_level()->master_key != nullptr && !game.get_player()->has_master_key) {
            if (game.get_level()->master_key->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)
                && game.get_level()->master_key->get_scene_object()->active
                && abs(game.get_player()->get_position().x - game.get_level()->master_key->get_position().x) <= 1
                && abs(game.get_player()->get_position().y - game.get_level()->master_key->get_position().y) <= 1) {
                game.get_level()->master_key->on_click();
                game.get_player()->has_master_key = true;
                animated_text_objects.push_back(FloatingNotificationText("Found Master Key", tutorial_font.value, glm::vec2(screen_dimensions.x / 2.0f - 100.0f, screen_dimensions.y / 2.0f + 50.0f), 0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 3.0f));
                sample_pickup->play(game.get_player()->get_position());
                return true;
            }
        }

        if(game.get_level()->dogTreatPickUp != nullptr && !game.get_player()->has_dog_treats) {
            if (game.get_level()->dogTreatPickUp->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)
                && game.get_level()->dogTreatPickUp->get_scene_object()->active
                && abs(game.get_player()->get_position().x - game.get_level()->dogTreatPickUp->get_position().x) <= 1
                && abs(game.get_player()->get_position().y - game.get_level()->dogTreatPickUp->get_position().y) <= 1) {
                game.get_level()->dogTreatPickUp->on_click();
                game.get_player()->has_dog_treats = true;
                animated_text_objects.push_back(FloatingNotificationText("Found Dog Treats", tutorial_font.value, glm::vec2(screen_dimensions.x / 2.0f - 100.0f, screen_dimensions.y / 2.0f + 50.0f), 0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 3.0f));
                return true;
            }
        }

        if(game.get_level()->cardboard_box != nullptr && !game.get_player()->has_cardboard_box) {
            if (game.get_level()->cardboard_box->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)
                && game.get_level()->cardboard_box->get_scene_object()->active
                && abs(game.get_player()->get_position().x - game.get_level()->cardboard_box->get_position().x) <= 1
                && abs(game.get_player()->get_position().y - game.get_level()->cardboard_box->get_position().y) <= 1) {
                game.get_level()->cardboard_box->on_click();
                game.get_player()->has_cardboard_box = true;
                animated_text_objects.push_back(FloatingNotificationText("Found Box Disguise", tutorial_font.value, glm::vec2(screen_dimensions.x / 2.0f - 100.0f, screen_dimensions.y / 2.0f + 50.0f), 0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 3.0f));
                return true;
            }
        }

        #ifdef GUARD_DEBUG
        bool guard_clicked = false;
        for (auto &guard : guards) {
            if (guard->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction) && !guard_clicked) {
                std::cout << "GUARD_DEBUG:: Guard (" << std::to_string(guard->get_instance_id())  << ") selected." << std::endl;
                guard->debug_focus = true;
                guard_clicked = true;
            } else if (guard->debug_focus) {
                guard->debug_focus = false;
            }
        }
        if (guard_clicked)
            return true;
        #endif

        for (uint32_t i = 0; i < game.get_level()->get_doors()->size(); i++) {
            if ((*game.get_level()->get_doors())[i]->get_boundingbox()->check_intersect(click_ray.origin, click_ray.direction)) {
                Door* door = (*game.get_level()->get_doors())[i];
                if (door->opened) {
                    
                    // Find the position in the next room
                    auto room_iter = door->rooms.find(game.get_player()->get_current_room());
                    if (room_iter!= door->rooms.end()) {
                        game.get_player()->set_path(Magpie::Navigation::getInstance().findPath(
                            glm::vec2(game.get_player()->get_position().x, game.get_player()->get_position().y),
                            glm::vec2(room_iter->second.x, room_iter->second.y)));
                        
                        game.get_player()->set_current_room(game.get_level()->get_tile_room_number((float)room_iter->second.x, (float)room_iter->second.y));
                        make_close_walls_transparent((float)room_iter->second.x, (float)room_iter->second.y);

                        if (game.get_player()->get_state() == (uint32_t)Player::STATE::IDLE) {
                            game.get_player()->set_state((uint32_t)Player::STATE::WALKING);
                        }
                        else if (game.get_player()->get_state() == (uint32_t)Player::STATE::DISGUISE_IDLE) {
                            game.get_player()->set_state((uint32_t)Player::STATE::DISGUISE_WALK);
                        }


                        return true;
                    }
                } else {
                    if ((door->access_level == Door::ACCESS_LEVEL::PINK && game.get_player()->has_pink_card)
                                || (door->access_level == Door::ACCESS_LEVEL::GREEN && game.get_player()->has_green_card)
                                || door->access_level == Door::ACCESS_LEVEL::NORMAL){

                        (*game.get_level()->get_doors())[i]->on_click();
                        animated_scene_objects.push_back((*game.get_level()->get_doors())[i]);

                        sample_door->play(door->get_position());
                        game.get_level()->set_movement_matrix_position((uint32_t)door->get_position().x, (uint32_t)door->get_position().y, true);

                        return true;

                    } else {
                        sample_fail->play(door->get_position());
                        animated_text_objects.push_back(FloatingNotificationText("Locked", tutorial_font.value, glm::vec2(screen_dimensions.x / 2.0f - 30.0f, screen_dimensions.y / 2.0f + 30.0f), 0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f));
                        return true;
                    }
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

            if (click_floor_intersect == game.get_player()->final_destination) return true;

            game.get_player()->final_destination = click_floor_intersect;

            Path path = Magpie::Navigation::getInstance().findPath(
                glm::vec2(game.get_player()->get_position().x, game.get_player()->get_position().y),
                glm::vec2(click_floor_intersect.x, click_floor_intersect.y));
            
            if (path.get_path().size() > 0) {

                make_close_walls_transparent((float)click_floor_intersect.x, (float)click_floor_intersect.y);
                game.get_player()->set_current_room(game.get_level()->get_tile_room_number((float)click_floor_intersect.x, (float)click_floor_intersect.y));
                game.get_player()->set_path(path);

                if (game.get_player()->get_state() == (uint32_t)Player::STATE::IDLE) {
                    game.get_player()->set_state((uint32_t)Player::STATE::WALKING);
                }
                else if (game.get_player()->get_state() == (uint32_t)Player::STATE::DISGUISE_IDLE) {
                    game.get_player()->set_state((uint32_t)Player::STATE::DISGUISE_WALK);
                }
                return true;

            }
            

        } else {
            //std::cout << "DEBUG:: Player can't move to that position" << std::endl;

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
            handled =  handle_clickables(click_ray);
        }

        if (!handled) {
            handled = handle_player_movement(get_click_floor_intersect(click_ray, 0.0f));
        }
        return handled;
    };

    void MagpieGameMode::show_tutorial() {
        std::shared_ptr< TutorialMode > tutorial = std::make_shared< TutorialMode >();

        std::shared_ptr< Mode > game = shared_from_this();
        tutorial->background = game;

        Mode::set_current(tutorial);
    }

}