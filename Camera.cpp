//
// In this camera code, the camera always looks at the origin and rotates around it with mouse input.
// 
// In order to implement free look behavior: 
//	-In the mouse function, do not change camPos.
// 	-Make direction a member variable of the Camera class, and use it as below.
//	-Change the 2nd argument of the lookAt calls to camPos + direction. (2nd argument is the target)
//	-Initialize Yaw to -90.0f (probably in Camera.h)
//   This is because direction looks towards the positive x-axis when yaw is 0.
//	

#include "Camera.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define UP glm::vec3(0.0f, 1.0f, 0.0f)
#define ORIGIN glm::vec3(0.0f, 0.0f, 0.0f)

//bool firstMouse = true;

void Camera::reshape(int w, int h) {
	w = w < 1 ? 1 : w;
	h = h < 1 ? 1 : h;

	Width = w;
	Height = h;

	glViewport(0, 0, Width, Height);

	// Use perspective projection
	float fovyRad = (float) (90.0 / 180.0) * M_PI;
	projectionMatrix = glm::perspective(fovyRad, (float) Width / Height, 0.1f, 100.0f);

	viewingMatrix = glm::lookAt(camPos, ORIGIN, UP);

	firstMouse = true;
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
}

void Camera::mouse_pos(GLFWwindow* window, double xpos, double ypos) {
	static float LastX, LastY;
	if (!RMB)
		return;
	if (firstMouse) {
		LastX = xpos;
		LastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - LastX;
	float yoffset = LastY - ypos; // reversed since y-coordinates go from bottom to top

	LastX = xpos;
	LastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Yaw += xoffset;
	Pitch -= yoffset;

	if (Pitch > 89.0f) {
		Pitch = 89.0f;
	}
	if (Pitch < -89.0) {
		Pitch = -89.0f;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	direction.y = sin(glm::radians(Pitch));
	direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	camPos = Distance * glm::normalize(direction);

	viewingMatrix = glm::lookAt(camPos, ORIGIN, UP);
}

void Camera::mouse_button(GLFWwindow* window, int button, int action) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			RMB = true;
		} else if (action == GLFW_RELEASE) {
			RMB = false;
		}
	}
}
