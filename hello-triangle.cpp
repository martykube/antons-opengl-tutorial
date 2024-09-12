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

void read_file(const char * file, std::string & content) {
    std::ifstream ifs(file);
    std::string line;
    while(std::getline(ifs, line)) {
        content += line + "\n";
    }
}

void error_callback(int error, const char* description) {
    gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
}

int main() {
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
    // GLFWwindow* window = glfwCreateWindow(vmode->width, vmode->height, "Extended GL Init", monitor, NULL);
    GLFWwindow* window = glfwCreateWindow(vmode->width, vmode->height, "HelloWindow", NULL, NULL);
    if(!window){
        fprintf(stderr, "Failed to open GLFW3 window\n");
        glfwTerminate();
        return 3;
    }
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    glewInit();

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);
    log_gl_params();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Other stuff goes here*/
    float size = 0.2;
    GLfloat points [] = {
         size,  size, 0.0f,
        -size, -size, 0.0f,
        -size,  size, 0.0f,
    };
    float offset = 1;
    GLfloat points2 [] = {
         size + offset,  size, 0.0f,
         size + offset, -size, 0.0f,
        -size + offset, -size, 0.0f,
    };

    // points
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

    // points2
    GLuint vbo2 = 0;
    glGenBuffers(1, &vbo2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points2), points2, GL_STATIC_DRAW);

    GLuint vao2 = 0;
    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

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

        glBindVertexArray(vao2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // update other events like input handling
        glfwPollEvents();
        
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
        // usleep(10000);
        if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }
    }


    glfwTerminate();
    return 0;
}
