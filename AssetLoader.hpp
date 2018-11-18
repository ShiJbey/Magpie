#pragma once

#include "Load.hpp"
#include "GL.hpp"
#include "MeshBuffer.hpp"
#include "TransformAnimation.hpp"
#include "GameCharacter.hpp"

#include <string>
#include <map>

namespace Magpie {
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

    // MAP OF VAO NAMES TO VAO POINTERS
    extern Load< std::map< std::string, GLuint > > vertex_color_vaos;

    
}
