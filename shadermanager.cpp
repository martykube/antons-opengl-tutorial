#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <iostream>
#include "logging.h"
#include "shadermanager.h"


void read_file(const char * file, std::string & content) {
    std::ifstream ifs(file);
    std::string line;
    while(std::getline(ifs, line)) {
        content += line + "\n";
    }
}

void check_for_shader_compile_error(GLuint vs) {
    GLint params = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
    if(GL_TRUE != params) {
        gl_log_err("ERROR: GL shader index %i did not compile\n", vs);
        int max_length = 2048;
        int actual_length = 0;
        char log[2048];
        glGetShaderInfoLog(vs, max_length, &actual_length, log);
        gl_log("shader info log for GL index %u:\n%s\n", vs, log);
        exit(22);
    }
}

void check_for_shader_link_error(GLuint shader_programme) {
    GLint params = -1;
    glGetProgramiv(shader_programme, GL_LINK_STATUS, &params);
    if(GL_TRUE != params) {
        gl_log_err("ERROR: could not link shader programme GL index %u\n", shader_programme);
        int max_length = 2048;
        int actual_length = 0;
        char log[2048];
        glGetProgramInfoLog(shader_programme, max_length, &actual_length, log);
        gl_log("program info log for GL index %u:\n%s", shader_programme, log);
        exit(23);
    }
    // Dev only
    glValidateProgram(shader_programme);
    glGetProgramiv(shader_programme, GL_VALIDATE_STATUS, &params);
    if(GL_TRUE != params) {
        gl_log("program %i GL_VALIDATE_STATUS = GL_FALSE\n", shader_programme);
        exit(24);
    } else {
        gl_log("Program validated\n");
    }
}

const char * GL_type_to_string(GLenum type) {
    switch(type) {
        case GL_BOOL: return "bool";
        case GL_INT: return "int";
        case GL_FLOAT: return "float";
        case GL_FLOAT_VEC2: return "vec2";
        case GL_FLOAT_VEC3: return "vec3";
        case GL_FLOAT_VEC4: return "vec4";
        case GL_FLOAT_MAT2: return "mat2";
        case GL_FLOAT_MAT3: return "mat3";
        case GL_FLOAT_MAT4: return "mat4";
        case GL_SAMPLER_2D: return "sampler2D";
        case GL_SAMPLER_3D: return "sampler3D";
        case GL_SAMPLER_CUBE: return "samplerCube";
        case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
        default: break;
    }
    return "other";
}

void show_shader(GLuint program) {
    gl_log("----------------\nshader programme %i info:\n", program);
    int params = -1;
    glGetProgramiv(program, GL_LINK_STATUS, &params);
    gl_log("GL_LINK_STATUS = %i\n", params);
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &params);
    gl_log("GL_ATTACHED_SHADERS = %i\n", params);
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &params);
    for(GLuint i = 0; i < (GLuint)params; i++) {
        char name[64];
        int max_length = 64;
        int actual_length = 0;
        int size = 0;
        GLenum type;
        glGetActiveAttrib(program, i, max_length, &actual_length, &size, &type, name);
        if(size > 1) {
            for(int j = 0; j < size; j++) {
                char long_name[100];
                gl_log(long_name, "%s[%i]", name, j);
                int location = glGetAttribLocation(program, long_name);
                gl_log("  %i) type:%s name:%s location:%i\n", i, GL_type_to_string(type), long_name, location);
            }
        } else {
            int location = glGetAttribLocation(program, name);
            gl_log("  %i) type:%s name:%s location:%i\n", i, GL_type_to_string(type), name, location);
        }
    }
}


GLuint get_shader_program() {
    gl_log("\nLoading shaders:\n");
    std::string vertex_shader_file;
    read_file("./shader-minimal.vert", vertex_shader_file);
    const char * vertex_shader = vertex_shader_file.c_str();

    std::string fragment_shader_file;
    read_file("./shader-minimal.frag", fragment_shader_file);
    const char * fragment_shader = fragment_shader_file.c_str();
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    check_for_shader_compile_error(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    check_for_shader_compile_error(fs);

    GLuint current_shader = glCreateProgram();
    glAttachShader(current_shader, fs);
    glAttachShader(current_shader, vs);
    glLinkProgram(current_shader);
    check_for_shader_link_error(current_shader);
    show_shader(current_shader);
    return current_shader;
}

bool uniform_location(GLuint shader_programme, const char * uniform_name, 
    float r, float g, float b, float alpha) {
    GLint current_shader_programme;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader_programme);
    if(shader_programme != current_shader_programme) {
        gl_log("Current shader program name %i\n", shader_programme);
        gl_log("Current reported shader program name %i\n", current_shader_programme);
        gl_log_err("ERROR: shader program %i is not current\n", shader_programme);
        return false;
    }
    GLuint uniform_location = glGetUniformLocation(shader_programme, "input_color");
    glUniform4f(uniform_location, 0.0f, 0.0f, 1.0f, 1.0f);
    return true;
}
