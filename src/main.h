#ifndef _MAIN_H
#define _NAIN_H

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ft2build.h"
#include FT_FREETYPE_H
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif
#include "utils.h"

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

void Update();

void RenderText(char* text, float x, float y, float scale, float* color);

bool Error(char* msg);

#endif  // _NAIN_H