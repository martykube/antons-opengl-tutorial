#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>


GLuint get_shader_program();
bool uniform_location(GLuint shader_programme, const char * uniform_name, 
    float r, float g, float b, float alpha);

