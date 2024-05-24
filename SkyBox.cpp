#include "SkyBox.h"

#include <GL/glew.h>
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "externals.h"
#include "Object.h"
#include "Shader.h"
#include "Camera.h"

using namespace std;

void SkyBox::set_hdr_texture(unsigned int texture) {
	BaseHDRTexture = texture;
}

void SkyBox::init_skybox() {
	glGenTextures(1, &CubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapTexture);
	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 2048, 2048, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	Shader cubemapCreator;
	cubemapCreator.init_shaders("shaders/CubemapInitVS.glsl", "shaders/CubemapInitFS.glsl");
	cubemapCreator.activate();

	Object quad; // initialize quad, 1 unit away towards -z
	quad.parse_obj("obj/quad.obj");
	quad.init_VBO();

	glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -1.0));
	cubemapCreator.set_mat4("model", translate);

	// view matrix looks towards -z
	glm::mat4 view = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0));
	cubemapCreator.set_mat4("view", view);

	// 90 degree FOV with 1:1 aspect ratio allows for a 90 degree FOV in both x and y
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	cubemapCreator.set_mat4("projection", projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, BaseHDRTexture);
	cubemapCreator.set_int("hdrTexture", 0);
	
	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	
	// rotation to deal with positive X face (right face)
	glm::mat4 rotateToPosX = glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	// rotation to deal with negative X face (left face)
	glm::mat4 rotateToNegX = glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
	// rotation to deal with positive Y face (top face)
	glm::mat4 rotateToPosY = glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	rotateToPosY = glm::rotate(rotateToPosY, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
	// rotation to deal with negative Y face (bottom face)
	glm::mat4 rotateToNegY = glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	rotateToNegY = glm::rotate(rotateToNegY, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
	// rotation to deal with positive Z face (back face)
	glm::mat4 rotateToPosZ = glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
	// rotation to deal with negative Z face (front face)
	glm::mat4 rotateToNegZ = glm::mat4(1.0);

	// set up viewport for 2048x2048 (same size as texture)
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, 2048, 2048);

	// Do offscreen rendering
	// +X
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, CubemapTexture, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	cubemapCreator.set_mat4("rotation", rotateToPosX);
	quad.draw_model();
	assert(glGetError() == GL_NO_ERROR);
	// -X
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, CubemapTexture, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	cubemapCreator.set_mat4("rotation", rotateToNegX);
	quad.draw_model();
	// +Y
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, CubemapTexture, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	cubemapCreator.set_mat4("rotation", rotateToPosY);
	quad.draw_model();
	// -Y
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, CubemapTexture, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	cubemapCreator.set_mat4("rotation", rotateToNegY);
	quad.draw_model();
	// +Z
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, CubemapTexture, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	cubemapCreator.set_mat4("rotation", rotateToPosZ);
	quad.draw_model();
	// -Z
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, CubemapTexture, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	cubemapCreator.set_mat4("rotation", rotateToNegZ);
	quad.draw_model();

	// Restore viewport
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &FBO);

	skyBox.parse_obj("obj/cube.obj");
	skyBox.init_VBO();
	skyBoxShader.init_shaders("shaders/SkyboxVS.glsl", "shaders/SkyboxFS.glsl");
}

void SkyBox::render_skybox(Camera& cam) {
	glDepthMask(GL_FALSE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapTexture);
	skyBoxShader.activate();
	glm::mat4 view = glm::mat4(glm::mat3(cam.viewingMatrix));
	skyBoxShader.set_mat4("view", view);
	skyBoxShader.set_mat4("projection", cam.projectionMatrix);

	glm::mat4 rotateSkybox = glm::rotate(glm::mat4(1.0), skyBoxRotation, glm::vec3(0.0, 1.0, 0.0));
	skyBoxShader.set_mat4("skyboxRotation", rotateSkybox);

	skyBox.draw_model();
	glDepthMask(GL_TRUE);
}
