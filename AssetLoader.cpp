#include "AssetLoader.hpp"
#include "data_path.hpp"
#include "vertex_color_program.hpp"
#include "highlight_program.hpp"
#include "transparent_program.hpp"

namespace Magpie {

    // DONUT
    Load< MeshBuffer > donut_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("levels/donut.pnc"));
    });

    Load< GLuint > donut_mesh_vao(LoadTagDefault, [](){
        return new GLuint(donut_mesh->make_vao_for_program(vertex_color_program->program));
    });

    // ANIMATED DOORS
    Load< MeshBuffer > door_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("levels/door.pnc"));
    });

    Load< GLuint > door_mesh_vao(LoadTagDefault, [](){
        return new GLuint(door_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > door_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("levels/door.tanim"));
    });

    Load< Magpie::ModelData > door_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("levels/door.model"));
    });

    // VISIBLE BOUNDING BOXES
    Load< MeshBuffer > bounding_box_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("levels/bounding_box.pnc"));
    });

    Load< GLuint > bounding_box_mesh_vao(LoadTagDefault, [](){
        return new GLuint(bounding_box_mesh->make_vao_for_program(vertex_color_program->program));
    });

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

    Load< Magpie::ModelData > magpie_idle_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("magpie/magpie_idle.model"));
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

    Load< Magpie::ModelData > magpie_walk_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("magpie/magpie_walk.model"));
    });

    // PLAYER STEALING
    Load< MeshBuffer > magpie_steal_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie/magpie_steal.pnc"));
    });

    Load< GLuint > magpie_steal_mesh_vao(LoadTagDefault, [](){
        return new GLuint(magpie_steal_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > magpie_steal_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("magpie/magpie_steal.tanim"));
    });

    Load< Magpie::ModelData > magpie_steal_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("magpie/magpie_steal.model"));
    });

    // PLAYER PICKING
    Load< MeshBuffer > magpie_pick_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie/magpie_pick.pnc"));
    });

    Load< GLuint > magpie_pick_mesh_vao(LoadTagDefault, [](){
        return new GLuint(magpie_pick_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > magpie_pick_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("magpie/magpie_pick.tanim"));
    });

    Load< Magpie::ModelData > magpie_pick_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("magpie/magpie_pick.model"));
    });

    // PLAYER BREAKING
    Load< MeshBuffer > magpie_break_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie/magpie_break.pnc"));
    });

    Load< GLuint > magpie_break_mesh_vao(LoadTagDefault, [](){
        return new GLuint(magpie_break_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > magpie_break_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("magpie/magpie_break.tanim"));
    });

    Load< Magpie::ModelData > magpie_break_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("magpie/magpie_break.model"));
    });

    // PLAYER DISGUISE IDLE
    Load< MeshBuffer > magpie_disguise_idle_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie/magpieDisguise_idle.pnc"));
    });

    Load< GLuint > magpie_disguise_idle_mesh_vao(LoadTagDefault, [](){
        return new GLuint(magpie_disguise_idle_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > magpie_disguise_idle_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("magpie/magpieDisguise_idle.tanim"));
    });

    Load< Magpie::ModelData > magpie_disguise_idle_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("magpie/magpieDisguise_idle.model"));
    });

    // PLAYER DISGUISE WALKING
    Load< MeshBuffer > magpie_disguise_walk_mesh(LoadTagDefault, [](){
        return new MeshBuffer(data_path("magpie/magpieDisguise_walk.pnc"));
    });

    Load< GLuint > magpie_disguise_walk_mesh_vao(LoadTagDefault, [](){
        return new GLuint(magpie_disguise_walk_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > magpie_disguise_walk_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("magpie/magpieDisguise_walk.tanim"));
    });

    Load< Magpie::ModelData > magpie_disguise_walk_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("magpie/magpieDisguise_walk.model"));
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

    Load< Magpie::ModelData > guard_dog_patrol_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("guardDog/guardDog_patrol.model"));
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

    Load< Magpie::ModelData > guard_dog_chase_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("guardDog/guardDog_chase.model"));
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

    Load< Magpie::ModelData > guard_dog_alert_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("guardDog/guardDog_alert.model"));
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

    Load< Magpie::ModelData > guard_dog_cautious_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("guardDog/guardDog_cautious.model"));
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

    Load< Magpie::ModelData > guard_dog_confused_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("guardDog/guardDog_confused.model"));
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

    Load< Magpie::ModelData > guard_dog_idle_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("guardDog/guardDog_idle.model"));
    });

    // GUARD EATING
    Load< MeshBuffer > guard_dog_eat_mesh(LoadTagDefault, []() {
        return new MeshBuffer(data_path("guardDog/guardDog_eat.pnc"));
    });

    Load< GLuint > guard_dog_eat_vao(LoadTagDefault, []() {
        return new GLuint(guard_dog_eat_mesh->make_vao_for_program(vertex_color_program->program));
    });

    Load< TransformAnimation > guard_dog_eat_tanim(LoadTagDefault, []() {
        return new TransformAnimation(data_path("guardDog/guardDog_eat.tanim"));
    });

    Load< Magpie::ModelData > guard_dog_eat_model(LoadTagDefault, []() {
        return new Magpie::ModelData(data_path("guardDog/guardDog_eat.model"));
    });

    // Vaos that the vertex color program will use
    Load< std::map< std::string, GLuint > > vertex_color_vaos(LoadTagLate, []() {
        return new std::map< std::string, GLuint > {
            {"donut", *donut_mesh_vao},
            {"bounding_box", *bounding_box_mesh_vao},
            {"buildingTiles", *building_meshes_vao},
            {"door", *door_mesh_vao},
            {"magpieWalk", *magpie_walk_mesh_vao},
            {"magpieIdle", *magpie_idle_mesh_vao},
            {"magpieSteal", *magpie_steal_mesh_vao},
            {"magpiePick", *magpie_pick_mesh_vao},
            {"magpieBreak", *magpie_break_mesh_vao},
            {"magpieDisguiseIdle", *magpie_disguise_idle_mesh_vao},
            {"magpieDisguiseWalk", *magpie_disguise_walk_mesh_vao},
            {"guardPatrol", *guard_dog_patrol_vao},
            {"guardChase", *guard_dog_chase_vao},
            {"guardAlert", *guard_dog_alert_vao},
            {"guardCautious", *guard_dog_cautious_vao},
            {"guardConfused", *guard_dog_confused_vao},
            {"guardIdle", *guard_dog_idle_vao},
            {"guardEat", *guard_dog_eat_vao}
        };
    });
}
