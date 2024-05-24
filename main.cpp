#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#include "externals.h"
#include "helpers.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "HDRTexture.h"
#include "SkyBox.h"

#define WIDTH 800
#define HEIGHT 800
#define LIGHT_PROBE_SHADER 0
#define GLOSSY_SHADER 3
#define DISCO_SHADER 4

Shader BaseShader;
Shader LightProbeShader;
Shader MirrorShader;
Shader GlassShader;
Shader GlossyShader;
Shader DiscoShader;
vector<Shader> Shaders(5);
int activeShaderIndex = 0;

int specularStatus = 1;

Object BaseObject;

Camera OrbitCamera;

HDRTexture Texture;
int medianCutLevel = 0;

SkyBox Skybox;
float skyBoxRotation = 0;

Shader ToneMappingShader;
GLuint ToneMappingFBO;
unsigned int ToneMappingTexture;
Object SceneQuad;
float exposure = 0.18f;

static void user_setup() {
	glEnable(GL_DEPTH_TEST);

	//BaseObject.parse_obj("obj/cube24.obj");
	BaseObject.parse_obj("obj/sphere.obj");
	BaseObject.init_VBO();

	//BaseShader.init_shaders("shaders/vert.glsl", "shaders/frag.glsl");
	//Shaders[0] = BaseShader;
	LightProbeShader.init_shaders("shaders/LightProbeVS.glsl", "shaders/LightProbeFS.glsl");
	Shaders[0] = LightProbeShader;
	MirrorShader.init_shaders("shaders/MirrorVS.glsl", "shaders/SkyboxFS.glsl");
	Shaders[1] = MirrorShader;
	GlassShader.init_shaders("shaders/GlassVS.glsl", "shaders/SkyboxFS.glsl");
	Shaders[2] = GlassShader;
	GlossyShader.init_shaders("shaders/GlossyVS.glsl", "shaders/GlossyFS.glsl");
	Shaders[3] = GlossyShader;
	DiscoShader.init_shaders("shaders/DiscoVS.glsl", "shaders/DiscoFS.glsl");
	Shaders[4] = DiscoShader;

	//Texture.init_hdr_texture("hdr_maps/Test.hdr");
	//Texture.init_hdr_texture("hdr_maps/Snowstreet.hdr");
	Texture.init_hdr_texture("hdr_maps/Thumersbach.hdr");
	//Texture.init_hdr_texture("hdr_maps/Beach.hdr");
	Texture.init_sum_area_table();
	Texture.precompute_median_cut_levels(0, Texture.Width-1, 0, Texture.Height-1, 0);
	Texture.init_arrays();
	//Texture.print_light_regions();
	//Texture.print_color_sum_area_table();

	Skybox.BaseHDRTexture = Texture.HDRTexture;
	Skybox.init_skybox();
}

void tonemapping_setup() {
	glGenFramebuffers(1, &ToneMappingFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ToneMappingFBO);

	glGenTextures(1, &ToneMappingTexture);
	glBindTexture(GL_TEXTURE_2D, ToneMappingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ToneMappingTexture, 0);

	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	
	ToneMappingShader.init_shaders("shaders/TonemapVS.glsl", "shaders/TonemapFS.glsl");

	SceneQuad.parse_obj("obj/quad.obj");
	SceneQuad.init_VBO();
}

static void display() {
	glBindFramebuffer(GL_FRAMEBUFFER, ToneMappingFBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	Skybox.render_skybox(OrbitCamera);

	Shaders[activeShaderIndex].activate();
	Shaders[activeShaderIndex].set_camera_vars(OrbitCamera.viewingMatrix, OrbitCamera.projectionMatrix, OrbitCamera.camPos);
	glm::mat4 rotateSkybox = glm::rotate(glm::mat4(1.0), skyBoxRotation, glm::vec3(0.0, 1.0, 0.0));
	Shaders[activeShaderIndex].set_mat4("skyboxRotation", rotateSkybox);
	if (activeShaderIndex == LIGHT_PROBE_SHADER || activeShaderIndex == GLOSSY_SHADER || activeShaderIndex == DISCO_SHADER) {
		Shaders[activeShaderIndex].set_int("numLights", int(pow(2,medianCutLevel)));
		Shaders[activeShaderIndex].set_int("specularStatus", specularStatus);
		Texture.pass_lights(medianCutLevel, Shaders[activeShaderIndex]);
	}
	if (activeShaderIndex == GLOSSY_SHADER) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, Skybox.CubemapTexture);
	}

	//float cubeAngle = 0;
	//float cubeAngleRad = (float) (cubeAngle / 180.0) * M_PI;
	//glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
	//glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0), cubeAngleRad, glm::vec3(0.0, 1.0, 0.0));
	//glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0), glm::vec3(1.0, 1.0, 1.0));
	//BaseObject.modelingMatrix = translationMatrix * rotationMatrix * scalingMatrix;
	//BaseObject.modelingMatrix = translationMatrix * rotationMatrix * scalingMatrix;
	BaseObject.modelingMatrix = glm::mat4(1.0);

	Shaders[activeShaderIndex].set_object_vars(BaseObject.modelingMatrix);
	BaseObject.draw_model();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	ToneMappingShader.activate();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ToneMappingTexture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTextureBarrier();

	ToneMappingShader.set_int("hdrScene", 0);
	ToneMappingShader.set_float("exposure", exposure);


	SceneQuad.draw_model();
}

int main(int argc, char** argv) {  // Create Main Function For Bringing It All Together
	GLFWwindow* window = init_window(WIDTH, HEIGHT, &OrbitCamera);
	init_glew();
	user_setup();
	tonemapping_setup();
	
	while (!glfwWindowShouldClose(window)) {
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
