#include "helpers.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "externals.h"
#include "Camera.h"

GLFWwindow* init_window(int width, int height, Camera* cam) {
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow *window = glfwCreateWindow(width, height, "MyWindow", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	char rendererInfo[512] = {0};
	strcpy(rendererInfo, (const char*) glGetString(GL_RENDERER));
	strcat(rendererInfo, " - ");
	strcat(rendererInfo, (const char*) glGetString(GL_VERSION));
	glfwSetWindowTitle(window, rendererInfo);
	
	glfwSetWindowUserPointer(window, cam);
	glfwSetKeyCallback(window, keyboard_handler);
	glfwSetWindowSizeCallback(window, reshape_handler);
	glfwSetCursorPosCallback(window, mouse_pos_handler);
	glfwSetMouseButtonCallback(window, mouse_button_handler);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	cam->reshape(width, height);
	
	return window;
}

void init_glew() {
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		exit(-1);
	}
}

void keyboard_handler(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		if (medianCutLevel < 7)
			medianCutLevel++;
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		if (medianCutLevel > 0)
			medianCutLevel--;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		skyBoxRotation += glm::radians(10.0);
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		skyBoxRotation -= glm::radians(10.0);
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		specularStatus = !specularStatus;
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		exposure *= 2.0f;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		exposure /= 2.0f;
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		activeShaderIndex = 0;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		activeShaderIndex = 1;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		activeShaderIndex = 2;
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		activeShaderIndex = 3;
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
		activeShaderIndex = 4;
	}
}

void reshape_handler(GLFWwindow* window, int w, int h) {
	Camera* cam = (Camera*) glfwGetWindowUserPointer(window);
	cam->reshape(w, h);
}

void mouse_pos_handler(GLFWwindow* window, double xpos, double ypos) {
	Camera* cam = (Camera*) glfwGetWindowUserPointer(window);
	cam->mouse_pos(window, xpos, ypos);
}

void mouse_button_handler(GLFWwindow* window, int button, int action, int mods) {
	Camera* cam = (Camera*) glfwGetWindowUserPointer(window);
	cam->mouse_button(window, button, action);
}
