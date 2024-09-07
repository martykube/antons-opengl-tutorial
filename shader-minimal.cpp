#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <iostream>
#include "gllogging.h"


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
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
    if(!window) {
        gl_log_err("ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
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

    /* OTHER STUFF GOES HERE NEXT */
    std::string vertex_shader_file;
    read_file("./hello-triangle..vert", vertex_shader_file);
    const char * vertex_shader = vertex_shader_file.c_str();

    std::string fragment_shader_file;
    read_file("./hello-triangle..frag", fragment_shader_file);
    const char * fragment_shader = fragment_shader_file.c_str();
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);

    // close GL context and any other GLFW resources
    glfwTerminate();
    return 0;
}