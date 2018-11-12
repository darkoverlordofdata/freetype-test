#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

const int WIDTH = 600;
const int HEIGHT = 400;
const int VERSION = 300;
const char* PROFILE = "es";

GLFWwindow* window;
GLuint ID;
GLuint VAO;
GLuint VBO;

void Update();
void RenderText(char* text, int length, float x, float y, float scale, float* color);

typedef enum { false, true } bool;

typedef struct IVec2
{
    int X;
    int Y;

} IVec2;

typedef struct Character 
{
    GLuint TextureID;     // ID handle of the glyph texture
    IVec2 Size;         // Size of glyph
    IVec2 Bearing;      // Offset from baseline to left/top of glyph
    long Advance;       // Horizontal offset to advance to next glyph

} Character;

Character Characters[128];

int main(int argc, char** args) 
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VERSION/100);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, (VERSION%100)/10);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, 
        VERSION < 320 
            ? GLFW_OPENGL_ANY_PROFILE 
            : PROFILE == "core" 
                ? GLFW_OPENGL_CORE_PROFILE 
                : GLFW_OPENGL_COMPAT_PROFILE );

    window = glfwCreateWindow(WIDTH, HEIGHT, "FreeType", NULL, NULL);
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    glewInit();
    glGetError(); 

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);
    // Set OpenGL options
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float projection[16] = {
        0.0025f,    0.0f,       0.0f,       0.0f,
        0.0f,       0.003333f,  0.0f,       0.0f,
        0.0f,       0.0f,       -2.0f,      0.0f,
        -1.0f,      -1.0f,      -1.0f,      1.0f
    };
    ID = CreateProgram("assets/text.vs", "assets/text.frag");
    glUseProgram(ID);
    glUniformMatrix4fv(glGetUniformLocation(ID, "projection"), 1, GL_FALSE, projection);

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        printf("ERROR::FREETYPE: Could not init FreeType Library");

    FT_Face face;
    if (FT_New_Face(ft, "assets/LiberationSansBold.ttf", 0, &face))
        printf("ERROR::FREETYPE: Failed to load font");

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    for (unsigned char c = 0; c < 128; c++)
    {
        // if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("ERROR::FREETYTPE: Failed to load Glyph");
            continue;
        }
        // Generate texture
        GLuint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            (GLsizei)face->glyph->bitmap.width,
            (GLsizei)face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            (void*)face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        IVec2 size = { face->glyph->bitmap.width, face->glyph->bitmap.rows };
        IVec2 bearing = { face->glyph->bitmap_left, face->glyph->bitmap_top };
        Character character = { texture, size, bearing, face->glyph->advance.x };
        Characters[c] = character;

    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, (int)(4 * sizeof(GLfloat)), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    #if (__EMSCRIPTEN__)
    emscripten_set_main_loop(Update, -1, 0);
    #else
    while (!glfwWindowShouldClose(window)) Update();
    glfwTerminate();
    #endif
    return 0;
}

void Update()
{
    // Game loop
    #if (!__EMSCRIPTEN__)
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    #endif
    // Check and call events
    glfwPollEvents();

    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float v1[3] = { 0.5f, 0.8f, 0.2f };
    float v2[3] = { 0.3f, 0.7f, 0.9f };
    RenderText("This is sample text", 19, 25.0f, 25.0f, 1.0f, v1);
    RenderText("(C) LearnOpenGL.com", 19, 540.0f, 570.0f, 0.5f, v2);

    glfwSwapBuffers(window);

}

void RenderText(char* text, int length, float x, float y, float scale, float* color)
{
    // Activate corresponding render state	
    glUseProgram(ID);
    glUniform3f(glGetUniformLocation(ID, "textColor"), color[0], color[1], color[2]);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    // for (c = text.begin(); c != text.end(); c++) 
    for (int i=0; i<length; i++)
    {
        unsigned char c = text[i];
        Character ch = Characters[c];

        float xpos = x + ch.Bearing.X * scale;
        float ypos = y - (ch.Size.Y - ch.Bearing.Y) * scale;

        float w = ch.Size.X * scale;
        float h = ch.Size.Y * scale;
        // Update VBO for each character
        float vertices[24] = {
            xpos,     ypos + h,   0.0f, 0.0f,            
            xpos,     ypos,       0.0f, 1.0f,
            xpos + w, ypos,       1.0f, 1.0f,

            xpos,     ypos + h,   0.0f, 0.0f,
            xpos + w, ypos,       1.0f, 1.0f,
            xpos + w, ypos + h,   1.0f, 0.0f           
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
}
