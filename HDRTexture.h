#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include "Shader.h"

using namespace std;

struct DirectionalLight {
	glm::vec3 direction;
	glm::vec3 color;
	DirectionalLight() : direction(glm::vec3(0.0f)), color(glm::vec3(0.0f)) {}
	DirectionalLight(glm::vec3 dir, glm::vec3 col) : direction(dir), color(col) {}
};

class HDRTexture {
private:
	int Channels; // will most likely be 3

	float* data;
	vector<vector<glm::vec3>> ColorSumAreaTable;
	vector<vector<float>> EnergySumAreaTable;

	vector<vector<DirectionalLight>> LightRegions = vector<vector<DirectionalLight>>(8);
	DirectionalLight LightsArray[8][128];
public:
	int Height;
	int Width;
	unsigned int HDRTexture;
	unsigned int UBOs[8];
	
private:
public:
	void init_hdr_texture(const string& HDRfile);
	void init_sum_area_table();
	float find_inclination(int y);
	void precompute_median_cut_levels(int x_Left, int x_Right, int y_Top, int y_Bottom, int n);
	void init_arrays();
	void pass_lights(int n, Shader shader);
	void print_light_regions();
	void print_color_sum_area_table();
};
