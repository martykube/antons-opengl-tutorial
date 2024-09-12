#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <iostream>
#include "gllogging.h"

// window
int g_window_width = 640;
int g_window_height = 480;
// framebuffer
int g_fb_width = 640;
int g_fb_height = 480;

void gflw_window_size_callback(GLFWwindow* window, int width, int height) {
    gl_log("GLFW: window size: %i x %i\n", width, height);
    g_window_width = width;
    g_window_height = height;
}

void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    gl_log("GLFW: framebuffer size: %i x %i\n", width, height);
    g_fb_width = width;
    g_fb_height = height;
}

void glfw_error_callback(int error, const char* description) {
    gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
}

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
        printf("shader info log for GL index %u:\n%s\n", vs, log);
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
        printf("program info log for GL index %u:\n%s", shader_programme, log);
        exit(23);
    }
    // Dev only
    glValidateProgram(shader_programme);
    glGetProgramiv(shader_programme, GL_VALIDATE_STATUS, &params);
    if(GL_TRUE != params) {
        printf("program %i GL_VALIDATE_STATUS = GL_FALSE\n", shader_programme);
        exit(24);
    } else {
        printf("Program validated\n");
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
    printf("----------------\nshader programme %i info:\n", program);
    int params = -1;
    glGetProgramiv(program, GL_LINK_STATUS, &params);
    printf("GL_LINK_STATUS = %i\n", params);
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &params);
    printf("GL_ATTACHED_SHADERS = %i\n", params);
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
                sprintf(long_name, "%s[%i]", name, j);
                int location = glGetAttribLocation(program, long_name);
                printf("  %i) type:%s name:%s location:%i\n", i, GL_type_to_string(type), long_name, location);
            }
        } else {
            int location = glGetAttribLocation(program, name);
            printf("  %i) type:%s name:%s location:%i\n", i, GL_type_to_string(type), name, location);
        }
    }
}


int main() {
    // start GL context and O/S window using the GLFW helper library
    if(!restart_gl_log()) {
        fprintf(stderr, "Failed to open log file\n");
        return 1;
    }
    gl_log("Starting GLFW: %s\n", glfwGetVersionString());
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW3\n");
        return 2;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Shader Minimal", NULL, NULL);
    // GLFWwindow* window = glfwCreateWindow(vmode->width, vmode->height, "HelloWindow", NULL, NULL);
    if(!window) {
        gl_log_err("ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwSetWindowSizeCallback(window, gflw_window_size_callback);
    glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    // start GLEW extension handler
    gl_log("GLEW: %s\n", glewGetString(GLEW_VERSION));
    glewExperimental = GL_TRUE;
    glewInit();

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    gl_log("Renderer: %s\n", renderer);
    gl_log("OpenGL version supported %s\n", version);
    log_gl_params();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Other stuff goes here*/
    // points
    float size = 0.2;
    GLfloat points [] = {
         size,  size, 0.0f,
        -size, -size, 0.0f,
        -size,  size, 0.0f,
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

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

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);
    check_for_shader_link_error(shader_programme);
    show_shader(shader_programme);

    GLuint uniform_location = glGetUniformLocation(shader_programme, "input_color");

    glUseProgram(shader_programme);
    glUniform4f(uniform_location, 0.0f, 1.0f, 0.0f, 1.0f);

    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
        _update_fps_counter(window);
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, g_fb_width, g_fb_height);

        glUseProgram(shader_programme);
        
        // draw points 0-3 from the currently bound VAO with current in-use shader
        glBindVertexArray(vao);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glPointSize(40.0f);
        glDrawArrays(GL_POINTS, 0, 3);

        // update other events like input handling
        glfwPollEvents();
        
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
        usleep(10000);
        if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    // close GL context and any other GLFW resources
    glfwTerminate();
    return 0;
}