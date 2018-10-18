#pragma once 

// This is supposed to fix linker errors generated when
// multiple source files have global Load variables

#include "Load.hpp"
#include "data_path.hpp"
#include "MeshBuffer.hpp"
#include "TransformAnimation.hpp"
#include "vertex_color_program.hpp"


extern Load< MeshBuffer > magpie_player_meshes(LoadTagDefault, [](){
	return new MeshBuffer(data_path("magpie.pnc"));
});

extern Load< GLuint > magpie_player_meshes_for_vertex_color_program(LoadTagDefault, [](){
	return new GLuint(magpie_player_meshes->make_vao_for_program(vertex_color_program->program));
});

extern Load< TransformAnimation > player_walk_tanim(LoadTagDefault, []() {
	return new TransformAnimation(data_path("player_walk.tanim"));
});

extern Load< TransformAnimation > player_idle_tanim(LoadTagDefault, []() {
	return new TransformAnimation(data_path("player_idle.tanim"));
});

extern Load< TransformAnimation > player_steal_tanim(LoadTagDefault, []() {
	return new TransformAnimation(data_path("player_steal.tanim"));
});