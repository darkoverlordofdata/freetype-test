#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <GL/glew.h>

char* ReadTextFile(const char* path);
GLuint CreateShader(const char* filename, GLenum type);
GLuint CreateProgram(const char* vertexfile, const char *fragmentfile);

#endif