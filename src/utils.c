#include <stdlib.h>
#include "utils.h"


void PrintLog(GLuint object);

char* ReadTextFile(const char* path)
{
    char* buf = NULL;
    FILE *f = fopen(path, "rb");
    if (f != NULL)
    {
        fseek(f, 0L, SEEK_END);
        long s = ftell(f);
        rewind(f);
        buf = malloc(s+1);
		buf[s] = '\0';

        if (buf != NULL)
        {
            fread(buf, s, 1, f);
            fclose(f);
            return buf;
        }
    }
    return buf;
}

GLuint CreateShader(const char* filename, GLenum type)
{
	const GLchar* source = ReadTextFile(filename);
	if (source == NULL) {
		printf("Error opening %s", filename);
		return 0;
	}
	GLuint shader = glCreateShader(type);
	const char* version = "#version 300 es\n";
	const char* precision =
        "#ifdef GL_ES                       \n"
        "precision mediump float;           \n"
        "#endif                             \n";

	const GLchar* sources[] = {
		version,
		precision,
		source
	};
	glShaderSource(shader, 3, sources, NULL);
	free((void*)source);
	
	glCompileShader(shader);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		printf("%s:\n", filename);
		PrintLog(shader);
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

GLuint CreateProgram(const char* vertexfile, const char *fragmentfile)
{
	GLuint program = glCreateProgram();
	GLuint shader;

	if(vertexfile) {
		shader = CreateShader(vertexfile, GL_VERTEX_SHADER);
		if(!shader)
			return 0;
		glAttachShader(program, shader);
	}

	if(fragmentfile) {
		shader = CreateShader(fragmentfile, GL_FRAGMENT_SHADER);
		if(!shader)
			return 0;
		glAttachShader(program, shader);
	}

	glLinkProgram(program);
	GLint link_ok = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		printf("glLinkProgram:");
		PrintLog(program);
		glDeleteProgram(program);
		return 0;
	}

	return program;
}


/**
 * Display compilation errors from the OpenGL shader compiler
 */
void PrintLog(GLuint object) {
	GLint log_length = 0;
	if (glIsShader(object)) {
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else if (glIsProgram(object)) {
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else {
		printf("printlog: Not a shader or a program");
		return;
	}

	char* log = (char*)malloc(log_length);
	
	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, log);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, log);
	
	printf("%s\n", log);
	free(log);
}
