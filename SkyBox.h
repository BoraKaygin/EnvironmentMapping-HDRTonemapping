#pragma once

#include <string>

#include "Object.h"
#include "Shader.h"
#include "Camera.h"

using namespace std;

class SkyBox {
private:
	Shader skyBoxShader;
	Object skyBox;
public:
	unsigned int BaseHDRTexture;
	unsigned int CubemapTexture;
	
private:
public:
	void set_hdr_texture(unsigned int texture);
	void init_skybox();
	void render_skybox(Camera& cam);
};
