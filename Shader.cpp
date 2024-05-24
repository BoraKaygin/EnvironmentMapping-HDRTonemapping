#include "Shader.h"

#include <fstream>
#include <iostream>

using namespace std;

bool Shader::ReadDataFromFile(const string& fileName, string& data) {
	fstream myfile;

	// Open the input 
	myfile.open(fileName.c_str(), std::ios::in);

	if (myfile.is_open()) {
		string curLine;

		while (getline(myfile, curLine)) {
			data += curLine;
			if (!myfile.eof()) {
				data += "\n";
			}
		}

		myfile.close();
	} else {
		return false;
	}

	return true;
}

GLuint Shader::CreateVS(const string& shaderName) {
	string shaderSource;

	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource)) {
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*) shaderSource.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &shader, &length);
	glCompileShader(vs);

	char output[1024] = {0};
	glGetShaderInfoLog(vs, 1024, &length, output);
	printf("VS compile log: %s\n", output);

	return vs;
}

GLuint Shader::CreateFS(const string& shaderName) {
	string shaderSource;

	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource)) {
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*) shaderSource.c_str();

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &shader, &length);
	glCompileShader(fs);

	char output[1024] = {0};
	glGetShaderInfoLog(fs, 1024, &length, output);
	printf("FS compile log: %s\n", output);

	return fs;
}

void Shader::init_shaders(const string& vertexSh, const string& fragmentSh) {
	// Create the programs
	Program = glCreateProgram();

	// Create the shaders for both programs
	GLuint vs1 = CreateVS(vertexSh);
	GLuint fs1 = CreateFS(fragmentSh);

	// Attach the shaders to the programs
	glAttachShader(Program, vs1);
	glAttachShader(Program, fs1);

	// Link the programs
	glLinkProgram(Program);
	GLint status;
	glGetProgramiv(Program, GL_LINK_STATUS, &status);

	if (status != GL_TRUE) {
		cout << "Program link failed" << endl;
		exit(-1);
	}

	glLinkProgram(Program);
	glGetProgramiv(Program, GL_LINK_STATUS, &status);

	if (status != GL_TRUE) {
		cout << "Program link failed" << endl;
		exit(-1);
	}
}

void Shader::activate() {
	glUseProgram(Program);
}

void Shader::set_mat4(const string& uniformName, glm::mat4 M) {
	activate();
	glUniformMatrix4fv(glGetUniformLocation(Program, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(M));
}

void Shader::set_vec3(const string& uniformName, glm::vec3 V) {
	activate();
	glUniform3fv(glGetUniformLocation(Program, uniformName.c_str()), 1, glm::value_ptr(V));
}

void Shader::set_int(const string& uniformName, GLint value) {
	activate();
	glUniform1i(glGetUniformLocation(Program, uniformName.c_str()), value);
}

void Shader::set_float(const string& uniformName, GLfloat value) {
	activate();
	glUniform1f(glGetUniformLocation(Program, uniformName.c_str()), value);
}

void Shader::set_camera_vars(glm::mat4 Mview, glm::mat4 Mproj, glm::vec3 Peye) {
	set_mat4("viewingMatrix", Mview);
	set_mat4("projectionMatrix", Mproj);
	set_vec3("eyePos", Peye);
}

void Shader::set_object_vars(glm::mat4 Mmodel) {
	set_mat4("modelingMatrix", Mmodel);
}
