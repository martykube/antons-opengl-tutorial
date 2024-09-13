#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <iostream>
#include "logging.h"
#include "shadermanager.h"


// GLFW callbacks
// window
int g_window_width = 640;
int g_window_height = 480;
// framebuffer
int g_fb_width = 640;
int g_fb_height = 480;

void window_size_callback(GLFWwindow* window, int width, int height) {
    gl_log("GLFW: window size: %i x %i\n", width, height);
    g_window_width = width;
    g_window_height = height;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    gl_log("GLFW: framebuffer size: %i x %i\n", width, height);
    g_fb_width = width;
    g_fb_height = height;
}

void error_callback(int error, const char* description) {
    gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
}

int main() {
    // start GL context and O/S window using the GLFW helper library
    if(!restart_gl_log()) {
        fprintf(stderr, "Failed to open log file\n");
        return 1;
    }
    gl_log("Starting GLFW: %s\n", glfwGetVersionString());
    glfwSetErrorCallback(error_callback);
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
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

    GLuint shader_programme = get_shader_program();
    glUseProgram(shader_programme);
    GLuint uniform_location = glGetUniformLocation(shader_programme, "input_color");
    glUniform4f(uniform_location, 1.0f, 0.0f, 0.0f, 1.0f);

    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
        update_fps_counter(window);
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, g_fb_width, g_fb_height);

        glUseProgram(shader_programme);
        GLuint uniform_location = glGetUniformLocation(shader_programme, "input_color");
        glUniform4f(uniform_location, 0.0f, 0.0f, 1.0f, 1.0f);
        
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

        if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE) ||
            GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Q)) {
            glfwSetWindowShouldClose(window, 1);
        }
        if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_R)) {
            shader_programme = get_shader_program();
        }
    }

    // close GL context and any other GLFW resources
    glfwTerminate();
    return 0;
}