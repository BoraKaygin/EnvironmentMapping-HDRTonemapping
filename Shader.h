#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <string>

using namespace std;

class Shader {
private:
	GLuint Program;
public:

private:
	bool ReadDataFromFile(
		const string& fileName, ///< [in]  Name of the shader file
		string& data);     ///< [out] The contents of the file
	GLuint CreateVS(const string& shaderName);
	GLuint CreateFS(const string& shaderName);
public:
	void init_shaders(const string& vertexSh, const string& fragmentSh);
	void activate();
	void set_mat4(const string& uniformName, glm::mat4 M);
	void set_vec3(const string& uniformName, glm::vec3 V);
	void set_int(const string& uniformName, GLint value);
	void set_float(const string& uniformName, GLfloat value);
	void set_camera_vars(glm::mat4 Mview, glm::mat4 Mproj, glm::vec3 Peye);
	void set_object_vars(glm::mat4 Mmodel);
};

