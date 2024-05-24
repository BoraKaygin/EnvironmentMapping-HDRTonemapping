#version 460 core

layout (location = 0) in vec3 InVertex;
layout (location = 1) in vec3 InNormal;

out vec4 color;

struct DirectionalLight {
	vec3 color;
	vec3 direction;
};

uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;
uniform vec3 eyePos;

uniform int numLights;
uniform DirectionalLight Lights[128];

vec3 kd = vec3(0.2, 0.2, 0.2);
vec3 ks = vec3(0.8, 0.8, 0.8);

void main() {
	vec4 pWorld = modelingMatrix * vec4(InVertex, 1.0);
	vec3 nWorld = inverse(transpose(mat3x3(modelingMatrix))) * InNormal;
	
	vec3 V = normalize(eyePos - vec3(pWorld));
	vec3 N = normalize(nWorld);

	vec3 colorSum = vec3(0.0);

	for (int i = 0; i < numLights; i++) {
		vec3 L = normalize(Lights[i].direction);
		vec3 R = reflect(-L, N);
		vec3 I = Lights[i].color;
		vec3 H = normalize(L + V);

		float NdotL = max(dot(N, L), 0.0);
		float NdotH = max(dot(N, H), 0.0);
		float VdotR = max(dot(V, R), 0.0);

		vec3 diffuseColor = I * kd * NdotL;
		//vec3 specularColor = I * ks * pow(NdotH, 100.0); // LIGHTPROBE
		vec3 specularColor = I * ks * pow(NdotH, 512.0); // DISCO

		//colorSum += diffuseColor + specularColor; // LIGHTPROBE
		colorSum += specularColor; // DISCO
	}

	color = vec4(colorSum, 1.0);

	gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(InVertex, 1.0);
}
