#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

class Camera {
private:
	int Width, Height;
	float Yaw = 90.0f, Pitch;
	float Distance = 5.0f;
	bool RMB = false;
	bool firstMouse = true;
public:
	glm::mat4 projectionMatrix;
	glm::mat4 viewingMatrix;
	glm::vec3 camPos = glm::vec3(0, 0, 5);

private:
public:
	void reshape(int w, int h);
	void mouse_pos(GLFWwindow* window, double xpos, double ypos);
	void mouse_button(GLFWwindow* window, int button, int action);
};

