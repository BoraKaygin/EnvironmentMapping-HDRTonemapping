#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Camera.h"

GLFWwindow* init_window(int width, int height, Camera* cam);
void init_glew();
void keyboard_handler(GLFWwindow* window, int key, int scancode, int action, int mods);
void reshape_handler(GLFWwindow* window, int w, int h);
void mouse_pos_handler(GLFWwindow* window, double xpos, double ypos);
void mouse_button_handler(GLFWwindow* window, int button, int action, int mods);
