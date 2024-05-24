#version 460 core
#define PI 3.14159265359

in vec3 direction;

out vec4 FragColor;

uniform sampler2D hdrTexture;

void main() {
	float theta = atan(direction.y, sqrt(direction.x*direction.x + direction.z*direction.z));
	float phi = atan(direction.z, direction.x);
	vec2 uv;
	uv.x = (phi + PI) / (2.0 * PI);
	uv.y = (theta + PI / 2.0) / PI;
	FragColor = texture(hdrTexture, uv);
}
