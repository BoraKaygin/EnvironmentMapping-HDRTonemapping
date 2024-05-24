#include "Object.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

bool Object::parse_obj(const string& fileName) {
	fstream myfile;
	myfile.open(fileName.c_str(), std::ios::in);
	if (myfile.is_open()) {
		string curLine;
		while (getline(myfile, curLine)) {
			stringstream str(curLine);
			GLfloat c1, c2, c3;
			//GLuint index[9];
			string tmp;
			if (curLine.length() >= 2) {
				if (curLine[0] == 'v') {
					if (curLine[1] == 't') { // texture
						str >> tmp; // consume "vt"
						str >> c1 >> c2;
						Textures.push_back(Texture(c1, c2));
					} else if (curLine[1] == 'n') { // normal
						str >> tmp; // consume "vn"
						str >> c1 >> c2 >> c3;
						Normals.push_back(Normal(c1, c2, c3));
					} else { // vertex
						str >> tmp; // consume "v"
						str >> c1 >> c2 >> c3;
						Vertices.push_back(Vertex(c1, c2, c3));
					}
				} else if (curLine[0] == 'f') { // face
					str >> tmp; // consume "f"
					char c;
					int vIndex[3]{}, nIndex[3]{}, tIndex[3]{};
					str >> vIndex[0]; str >> c >> c; // consume "//"
					str >> nIndex[0];
					str >> vIndex[1]; str >> c >> c; // consume "//"
					str >> nIndex[1];
					str >> vIndex[2]; str >> c >> c; // consume "//"
					str >> nIndex[2];

					assert(vIndex[0] == nIndex[0] &&
						   vIndex[1] == nIndex[1] &&
						   vIndex[2] == nIndex[2]); // a limitation for now

					for (int c = 0; c < 3; ++c) // make indices start from 0
					{
						vIndex[c] -= 1;
						nIndex[c] -= 1;
						tIndex[c] -= 1;
					}
					Faces.push_back(Face(vIndex, tIndex, nIndex));
				} else {
					cout << "Ignoring unidentified line in obj file: " << curLine << endl;
				}
			}
			/*
			//data += curLine;
			if (!myfile.eof())
			{
				//data += "\n";
			}
			*/
		}
		myfile.close();
	} else {
		return false;
	}

	assert(Vertices.size() == Normals.size());

	return true;
}

void Object::init_VBO() {
	glGenVertexArrays(1, &VAO);
	assert(VAO > 0);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	assert(glGetError() == GL_NONE);

	glGenBuffers(1, &VertexAttribBuffer);
	glGenBuffers(1, &IndexBuffer);

	assert(VertexAttribBuffer > 0 && IndexBuffer > 0);

	glBindBuffer(GL_ARRAY_BUFFER, VertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);

	VertexDataSizeInBytes = Vertices.size() * 3 * sizeof(GLfloat);
	NormalDataSizeInBytes = Normals.size() * 3 * sizeof(GLfloat);
	int indexDataSizeInBytes = Faces.size() * 3 * sizeof(GLuint);
	GLfloat* vertexData = new GLfloat[Vertices.size() * 3];
	GLfloat* normalData = new GLfloat[Normals.size() * 3];
	GLuint* indexData = new GLuint[Faces.size() * 3];

	float minX = 1e6, maxX = -1e6;
	float minY = 1e6, maxY = -1e6;
	float minZ = 1e6, maxZ = -1e6;

	for (int i = 0; i < Vertices.size(); ++i) {
		vertexData[3 * i] = Vertices[i].x;
		vertexData[3 * i + 1] = Vertices[i].y;
		vertexData[3 * i + 2] = Vertices[i].z;
		/*
		minX = std::min(minX, Vertices[i].x);
		maxX = std::max(maxX, Vertices[i].x);
		minY = std::min(minY, Vertices[i].y);
		maxY = std::max(maxY, Vertices[i].y);
		minZ = std::min(minZ, Vertices[i].z);
		maxZ = std::max(maxZ, Vertices[i].z);
		*/
	}

	/*
	std::cout << "minX = " << minX << std::endl;
	std::cout << "maxX = " << maxX << std::endl;
	std::cout << "minY = " << minY << std::endl;
	std::cout << "maxY = " << maxY << std::endl;
	std::cout << "minZ = " << minZ << std::endl;
	std::cout << "maxZ = " << maxZ << std::endl;
	*/

	for (int i = 0; i < Normals.size(); ++i) {
		normalData[3 * i] = Normals[i].x;
		normalData[3 * i + 1] = Normals[i].y;
		normalData[3 * i + 2] = Normals[i].z;
	}

	for (int i = 0; i < Faces.size(); ++i) {
		indexData[3 * i] = Faces[i].vIndex[0];
		indexData[3 * i + 1] = Faces[i].vIndex[1];
		indexData[3 * i + 2] = Faces[i].vIndex[2];
	}


	glBufferData(GL_ARRAY_BUFFER, VertexDataSizeInBytes + NormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, VertexDataSizeInBytes, vertexData);
	glBufferSubData(GL_ARRAY_BUFFER, VertexDataSizeInBytes, NormalDataSizeInBytes, normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	// done copying; can free now
	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(VertexDataSizeInBytes));

	glBindVertexArray(0);
}

void Object::draw_model() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, Faces.size() * 3, GL_UNSIGNED_INT, 0);
	assert(glGetError() == GL_NONE);
}
