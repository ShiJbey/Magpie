#include "ui_shader_program.hpp"

#include "compile_program.hpp"

UIProgram::UIProgram() {
	program = compile_program(
		"#version 330\n"
		"uniform mat4 projection;\n"
		"layout(location=0) in vec3 Position;\n" //note: layout keyword used to make sure that the location-0 attribute is always bound to something
		"void main() {\n"
		"	gl_Position = vec4(Position.xy, 0.0, 1.0);\n"
		"}\n"
		,
		"#version 330\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"	fragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
		"}\n"
	);

	projection_mat4 = glGetUniformLocation(program, "projection");
}

Load< UIProgram > ui_program(LoadTagInit, [](){
	return new UIProgram();
});
