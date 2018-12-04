#include "GL.hpp"
#include "Load.hpp"

struct UIProgram {
    //opengl program object:
	GLuint program = 0;

    GLuint projection_mat4 = -1U;

    UIProgram();
};

extern Load< UIProgram > ui_program;