#pragma once
#include <stdarg.h>

bool restart_gl_log();
bool gl_log(const char * message, ...);
bool gl_log_err(const char * message, ...);
void update_fps_counter(GLFWwindow* window);
void log_gl_params();