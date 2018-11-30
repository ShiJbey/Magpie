#pragma once

#include "Load.hpp"
#include "GL.hpp"
#include "MeshBuffer.hpp"
#include "load_level.hpp"

#include "TransformAnimation.hpp"
#include "AnimatedModel.hpp"


#include <string>
#include <map>

namespace Magpie {
    // DONUT
    extern Load< MeshBuffer > donut_mesh;

    extern Load< GLuint > donut_mesh_vao;

    // PLAIN DOOR
    extern Load< MeshBuffer > door_mesh;

    extern Load< GLuint > door_mesh_vao;

    extern Load< TransformAnimation > door_tanim;

    extern Load< Magpie::ModelData > door_model;

    // PINK DOOR
    extern Load< MeshBuffer > door_pink_mesh;

    extern Load< GLuint > door_pink_mesh_vao;

    extern Load< TransformAnimation > door_pink_tanim;

    extern Load< Magpie::ModelData > door_pink_model;

    // GREEN_DOOR
    extern Load< MeshBuffer > door_green_mesh;

    extern Load< GLuint > door_green_mesh_vao;

    extern Load< TransformAnimation > door_green_tanim;

    extern Load< Magpie::ModelData > door_green_model;

    // VISIBLE BOUNDING BOXES
    extern Load< MeshBuffer > bounding_box_mesh;

    extern Load< GLuint > bounding_box_mesh_vao;

    // BUILDING TILES
    extern Load< MeshBuffer > building_meshes;

    extern Load< GLuint > building_meshes_vao;

    extern Load< GLuint > highlighted_building_meshes_vao;

    extern Load< GLuint > transparent_building_meshes_vao;

    // PLAYER IDLE
    extern Load< MeshBuffer > magpie_idle_mesh;

    extern Load< GLuint > magpie_idle_mesh_vao;

    extern Load< TransformAnimation > magpie_idle_tanim;

    extern Load< Magpie::ModelData > magpie_idle_model;

    // PLAYER WALKING
    extern Load< MeshBuffer > magpie_walk_mesh;

    extern Load< GLuint > magpie_walk_mesh_vao;

    extern Load< TransformAnimation > magpie_walk_tanim;

    extern Load< Magpie::ModelData > magpie_walk_model;

    // PLAYER STEALING
    extern Load< MeshBuffer > magpie_steal_mesh;

    extern Load< GLuint > magpie_steal_mesh_vao;

    extern Load< TransformAnimation > magpie_steal_tanim;

    extern Load< Magpie::ModelData > magpie_steal_model;

    // PLAYER PICKING
    extern Load< MeshBuffer > magpie_pick_mesh;

    extern Load< GLuint > magpie_pick_mesh_vao;

    extern Load< TransformAnimation > magpie_pick_tanim;

    extern Load< Magpie::ModelData > magpie_pick_model;

    // PLAYER BREAKING
    extern Load< MeshBuffer > magpie_break_mesh;

    extern Load< GLuint > magpie_break_mesh_vao;

    extern Load< TransformAnimation > magpie_break_tanim;

    extern Load< Magpie::ModelData > magpie_break_model;

    // PLAYER DISGUISE IDLE
    extern Load< MeshBuffer > magpie_disguise_idle_mesh;

    extern Load< GLuint > magpie_disguise_idle_mesh_vao;

    extern Load< TransformAnimation > magpie_disguise_idle_tanim;

    extern Load< Magpie::ModelData > magpie_disguise_idle_model;

    // PLAYER DISGUISE WALKING
    extern Load< MeshBuffer > magpie_disguise_walk_mesh;

    extern Load< GLuint > magpie_disguise_walk_mesh_vao;

    extern Load< TransformAnimation > magpie_disguise_walk_tanim;

    extern Load< Magpie::ModelData > magpie_disguise_walk_model;

    // GUARD PATROLING
    extern Load< MeshBuffer > guard_dog_patrol_mesh;

    extern Load< GLuint > guard_dog_patrol_vao;

    extern Load< TransformAnimation > guard_dog_patrol_tanim;

    extern Load< Magpie::ModelData > guard_dog_patrol_model;

    // GUARD CHASING
    extern Load< MeshBuffer > guard_dog_chase_mesh;

    extern Load< GLuint > guard_dog_chase_vao;

    extern Load< TransformAnimation > guard_dog_chase_tanim;

    extern Load< Magpie::ModelData > guard_dog_chase_model;

    // GUARD ALERT
    extern Load< MeshBuffer > guard_dog_alert_mesh;

    extern Load< GLuint > guard_dog_alert_vao;

    extern Load< TransformAnimation > guard_dog_alert_tanim;

    extern Load< Magpie::ModelData > guard_dog_alert_model;

    // GUARD CAUTIOUS
    extern Load< MeshBuffer > guard_dog_cautious_mesh;

    extern Load< GLuint > guard_dog_cautious_vao;

    extern Load< TransformAnimation > guard_dog_cautious_tanim;

    extern Load< Magpie::ModelData > guard_dog_cautious_model;

    // GUARD CONFUSED
    extern Load< MeshBuffer > guard_dog_confused_mesh;

    extern Load< GLuint > guard_dog_confused_vao;

    extern Load< TransformAnimation > guard_dog_confused_tanim;

    extern Load< Magpie::ModelData > guard_dog_confused_model;

    // GUARD IDLE
    extern Load< MeshBuffer > guard_dog_idle_mesh;

    extern Load< GLuint > guard_dog_idle_vao;

    extern Load< TransformAnimation > guard_dog_idle_tanim;

    extern Load< Magpie::ModelData > guard_dog_idle_model;

    // GUARD EATING
    extern Load< MeshBuffer > guard_dog_eat_mesh;

    extern Load< GLuint > guard_dog_eat_vao;

    extern Load< TransformAnimation > guard_dog_eat_tanim;

    extern Load< Magpie::ModelData > guard_dog_eat_model;

    // MAP OF VAO NAMES TO VAO POINTERS
    extern Load< std::map< std::string, GLuint > > vertex_color_vaos;

    // LEVEL DATA
    extern Load< Magpie::LevelData > sample_map;
    extern Load< Magpie::LevelData > demo_map;
    extern Load< Magpie::LevelData > demo_map_simple;
    extern Load< Magpie::LevelData > final_map;

    // Program Information
    extern Load< Scene::Object::ProgramInfo > vertex_color_program_info;
    extern Load< Scene::Object::ProgramInfo > highlight_program_info;
    extern Load< Scene::Object::ProgramInfo > transparent_program_info;
}
