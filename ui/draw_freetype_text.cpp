#include "draw_freetype_text.hpp"
#include "../base/data_path.hpp"
#include "../base/Load.hpp"
#include "../base/compile_program.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H






GLuint ttf_text_program_text_sampler2d = -1U;
GLuint ttf_text_program_text_color_vf3 = -1U;
GLuint ttf_text_program_projection_mat4 = -1U;

Load< GLuint > ttf_text_program(LoadTagInit, [](){
    GLuint *ret = new GLuint(compile_program(
		"#version 330 core\n"
        "layout (location = 0) in vec4 vertex;\n" // <vec2 pos, vec2 tex>
        "out vec2 TexCoords;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
        "    TexCoords = vertex.zw;\n"
        "}\n"
	,
		"#version 330 core\n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"
        "uniform sampler2D text;\n"
        "uniform vec3 textColor;\n"
        "void main()\n"
        "{\n"
        "    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
        "    color = vec4(textColor, 1.0) * sampled;\n"
        "}\n"
	));

    ttf_text_program_projection_mat4 = glGetUniformLocation(*ret, "projection");
    ttf_text_program_text_sampler2d = glGetUniformLocation(*ret, "text");
    ttf_text_program_text_color_vf3 = glGetUniformLocation(*ret, "textColor");

    return ret;
});



std::map< GLchar, Character >* load_font(uint32_t font_height, const std::string &font_file) {
    std::map<GLchar, Character>* Characters = new std::map<GLchar, Character>();

    FT_Library ft;

    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_file.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, font_height);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (int)face->glyph->advance.x
        };
        Characters->insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return Characters;
};

Load< std::map< GLchar, Character > > ransom_font(LoadTagDefault, [](){
    return load_font(64, data_path("fonts/DJB Ransom Note Clipped.ttf"));
});

Load< std::map< GLchar, Character > > tutorial_font(LoadTagDefault, [](){
    return load_font(64, data_path("fonts/NeoBulletin Limited Free Version.ttf"));
});

void RenderText(const std::map< GLchar, Character >* font, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(*ttf_text_program);

    GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(viewport[2]), 0.0f, static_cast<GLfloat>(viewport[3]));
    glUniformMatrix4fv(ttf_text_program_projection_mat4, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3f(ttf_text_program_text_color_vf3, color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        auto pos = font->find(*c);
        Character ch = pos->second;

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

};