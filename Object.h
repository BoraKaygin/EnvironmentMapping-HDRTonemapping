#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <vector>
#include <string>

using namespace std;

struct Vertex {
	GLfloat x, y, z;
	Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) {}
};

struct Texture {
	GLfloat u, v;
	Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) {}
};

struct Normal {
	GLfloat x, y, z;
	Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) {}
};

struct Face {
	GLuint vIndex[3], tIndex[3], nIndex[3];
	Face(int v[], int t[], int n[]) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
};

class Object {
private:
	vector<Vertex> Vertices;
	vector<Texture> Textures;
	vector<Normal> Normals;
	vector<Face> Faces;
	GLuint VAO;
	GLuint VertexAttribBuffer, IndexBuffer;
	GLint InVertexLoc, InNormalLoc;
	int VertexDataSizeInBytes, NormalDataSizeInBytes;
public:
	glm::mat4 modelingMatrix;

private:
public:
	bool parse_obj(const string& fileName);
	void init_VBO();
	void draw_model();
};
