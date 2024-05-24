#include "HDRTexture.h"

#include <GL/glew.h>
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include "stb_image.h"

#include "Shader.h"


void HDRTexture::init_hdr_texture(const string& HDRfile) {
	data = stbi_loadf(HDRfile.c_str(), &Width, &Height, &Channels, 0);
	if (data) {
		glGenTextures(1, &HDRTexture);
		glBindTexture(GL_TEXTURE_2D, HDRTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Width, Height, 0, GL_RGB, GL_FLOAT, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//stbi_image_free(data);
	} else {
		std::cout << "Failed to load HDR image." << std::endl;
	}
}

void HDRTexture::init_sum_area_table() {
	ColorSumAreaTable.resize(Height);
	EnergySumAreaTable.resize(Height);
	for (int y = 0; y < Height; y++) {
		ColorSumAreaTable[y].resize(Width);
		EnergySumAreaTable[y].resize(Width);
	}

	float cosInc = abs(cos(find_inclination(0)));
	ColorSumAreaTable[0][0] = glm::vec3(cosInc * data[0], cosInc * data[1], cosInc * data[2]);

	for (int y = 1; y < Height; y++) {
		int index = y * Width * Channels;
		cosInc = abs(cos(find_inclination(y)));
		ColorSumAreaTable[y][0] = ColorSumAreaTable[y - 1][0] + glm::vec3(cosInc * data[index], cosInc * data[index + 1], cosInc * data[index + 2]);
	}

	for (int x = 1; x < Width; x++) {
		int index = x * Channels;
		cosInc = abs(cos(find_inclination(0)));
		ColorSumAreaTable[0][x] = ColorSumAreaTable[0][x - 1] + glm::vec3(cosInc * data[index], cosInc * data[index + 1], cosInc * data[index + 2]);
	}

	for (int y = 1; y < Height; y++) {
		cosInc = abs(cos(find_inclination(y)));
		for (int x = 1; x < Width; x++) {
			int index = y * Width * Channels + x * Channels;
			ColorSumAreaTable[y][x] = ColorSumAreaTable[y - 1][x] + ColorSumAreaTable[y][x - 1] - ColorSumAreaTable[y - 1][x - 1] + glm::vec3(cosInc * data[index], cosInc * data[index + 1], cosInc * data[index + 2]);
		}
	}

	for (int y = 0; y < Height; y++) {
		for (int x = 0; x < Width; x++) {
			// (0.2126, 0.7152, 0.0722) 
			EnergySumAreaTable[y][x] = 0.2126*ColorSumAreaTable[y][x].r + 0.7152*ColorSumAreaTable[y][x].g + 0.0722*ColorSumAreaTable[y][x].b;
		}
	}
}

float HDRTexture::find_inclination(int y) {
	float halfHeight = Height / 2.0;
	float dist = abs((float) y - halfHeight);
	float inclinationDegree = 90.0 * dist / halfHeight;
	float inclinationRadian = inclinationDegree / 180.0 * M_PI;
	return inclinationRadian;
}

void HDRTexture::precompute_median_cut_levels(int x_Left, int x_Right, int y_Top, int y_Bottom, int n) {
	if (n >= 8)
		return;
	float E_Left = x_Left > 0 ? EnergySumAreaTable[y_Bottom][x_Left - 1] : 0.0;
	float E_Top = y_Top > 0 ? EnergySumAreaTable[y_Top - 1][x_Right] : 0.0;
	float E_TopLeft = x_Left > 0 && y_Top > 0 ? EnergySumAreaTable[y_Top - 1][x_Left - 1] : 0.0;
	float E_Total = EnergySumAreaTable[y_Bottom][x_Right];
	float E_Region = E_Total - E_Left - E_Top + E_TopLeft;
	float E_Half = E_Region / 2.0;

	float x_Centroid = (float) (x_Right - x_Left) / 2; 
	float y_Centroid = (float) (y_Bottom - y_Top) / 2;
	//float x_Centroid, y_Centroid;
	for (int y = y_Top; y <= y_Bottom; y++) {
		float E_curLeft = x_Left > 0 ? EnergySumAreaTable[y][x_Left - 1] : 0.0;
		float E_curTotal = EnergySumAreaTable[y][x_Right];
		float E_curRegion = E_curTotal - E_curLeft - E_Top + E_TopLeft;
		if (E_curRegion >= E_Half) {
			y_Centroid = y;
			break;
		}
	}
	for (int x = x_Left; x <= x_Right; x++) {
		float E_curTop = y_Top > 0 ? EnergySumAreaTable[y_Top - 1][x] : 0.0;
		float E_curTotal = EnergySumAreaTable[y_Bottom][x];
		float E_curRegion = E_curTotal - E_Left - E_curTop + E_TopLeft;
		if (E_curRegion >= E_Half) {
			x_Centroid = x;
			break;
		}
	}
	
	glm::vec3 C_Left = x_Left > 0 ? ColorSumAreaTable[y_Bottom][x_Left - 1] : glm::vec3(0.0);
	glm::vec3 C_Top = y_Top > 0 ? ColorSumAreaTable[y_Top - 1][x_Right] : glm::vec3(0.0);
	glm::vec3 C_TopLeft = x_Left > 0 && y_Top > 0 ? ColorSumAreaTable[y_Top - 1][x_Left - 1] : glm::vec3(0.0);
	glm::vec3 C_Total = ColorSumAreaTable[y_Bottom][x_Right];
	glm::vec3 C_Region = C_Total - C_Left - C_Top + C_TopLeft;
	C_Region /= (x_Right - x_Left + 1) * (y_Bottom - y_Top + 1);

	float phi = M_PI * (2.0 * (float) x_Centroid/Width - 1.0);
	float theta = M_PI * ((float) y_Centroid/Height - 0.5);
	glm::vec3 centroidDirection(1, 0, 0);
	glm::mat4 heightRotation = glm::rotate(glm::mat4(1.0), theta, glm::vec3(0, 0, 1));
	glm::mat4 widthRotation = glm::rotate(glm::mat4(1.0), phi, glm::vec3(0, 1, 0));
	centroidDirection = glm::vec3(widthRotation * heightRotation * glm::vec4(centroidDirection, 0.0));
	centroidDirection.y *= -1;
	LightRegions[n].emplace_back(centroidDirection, C_Region);

	int regionWidth = (float) (x_Right - x_Left + 1) * cos(find_inclination((y_Bottom - y_Top)/2.0));
	int regionHeight = y_Bottom - y_Top + 1;
	if (regionWidth > regionHeight) {
		precompute_median_cut_levels(x_Left, x_Centroid, y_Top, y_Bottom, n + 1);
		precompute_median_cut_levels(x_Centroid + 1, x_Right, y_Top, y_Bottom, n + 1);
	} else {
		precompute_median_cut_levels(x_Left, x_Right, y_Top, y_Centroid, n + 1);
		precompute_median_cut_levels(x_Left, x_Right, y_Centroid + 1, y_Bottom, n + 1);
	}
}

void HDRTexture::init_arrays() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < LightRegions[i].size(); j++) {
			LightsArray[i][j] = LightRegions[i][j];
		}
	}
	/*
	for (int i = 0; i < 128; i++) {
		std::cout << "Direction: (" << LightsArray[1][i].direction.x << ", " << LightsArray[1][i].direction.y << ", " << LightsArray[1][i].direction.z << ")" << std::endl;
		std::cout << "Color: (" << LightsArray[1][i].color.r << ", " << LightsArray[1][i].color.g << ", " << LightsArray[1][i].color.b << ")" << std::endl;
	}
	*/
	/*
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 128; j++) {
			string baseName = "Lights[" + to_string(i) + "][" + to_string(j) + "]";
			sh.set_vec3(baseName + ".direction", LightsArray[i][j].direction);
			sh.set_vec3(baseName + ".color", LightsArray[i][j].color);
		}
	}
	*/
}

void HDRTexture::pass_lights(int n, Shader shader) {
	for (int i = 0; i < 128; i++) {
		string baseName = "Lights[" + to_string(i) + "]";
		shader.set_vec3(baseName + ".direction", LightsArray[n][i].direction);
		shader.set_vec3(baseName + ".color", LightsArray[n][i].color);
	}
}

void HDRTexture::print_light_regions() {
	for (int i = 0; i < 8; i++) {
		std::cout << "Region " << i << std::endl;
		for (int j = 0; j < LightRegions[i].size(); j++) {
			std::cout << "Direction: (" << LightRegions[i][j].direction.x << ", " << LightRegions[i][j].direction.y << ", " << LightRegions[i][j].direction.z << ")" << std::endl;
			std::cout << "Color: (" << LightRegions[i][j].color.r << ", " << LightRegions[i][j].color.g << ", " << LightRegions[i][j].color.b << ")" << std::endl;
		}
	}
}

void HDRTexture::print_color_sum_area_table() {
	for (int y = 0; y < Height; y++) {
		for (int x = 0; x < Width; x++) {
			printf("(%f.1, %f.1, %f.1) \n", ColorSumAreaTable[y][x].r, ColorSumAreaTable[y][x].g, ColorSumAreaTable[y][x].b);
		}
		std::cout << std::endl;
	}
}
