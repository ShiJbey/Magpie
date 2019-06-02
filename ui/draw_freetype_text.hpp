#pragma once

#include <iostream>
#include <map>
#include <string>
#include "../base/Load.hpp"

#include <glm/glm.hpp>
#include "../base/GL.hpp"

// Much of this code is borrowed from:
// https://learnopengl.com/code_viewer.php?code=in-practice/text_rendering

//Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    GLint Advance;
};

extern Load< std::map< GLchar, Character > > ransom_font;
extern Load< std::map< GLchar, Character > > tutorial_font;

// Probably need to Load the freetype library with the Load Class
std::map< GLchar, Character >* load_font(uint32_t font_height, const std::string &font_file);

// Renders text to the view port
void RenderText(const std::map< GLchar, Character >* font, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);