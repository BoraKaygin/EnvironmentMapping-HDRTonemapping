#version 460 core

in vec4 pWorld;
in vec3 nWorld;

out vec4 fragColor;

struct DirectionalLight
{
	vec3 direction;
	vec3 color;
};

uniform vec3 eyePos;

uniform int numLights;
uniform DirectionalLight Lights[128];

uniform int specularStatus;

uniform samplerCube skybox;

vec3 kd = vec3(0.3, 0.3, 0.3);
vec3 ks = vec3(0.8, 0.8, 0.8);

uniform mat4 skyboxRotation;

void main(void)
{
	vec3 V = normalize(eyePos - vec3(pWorld));
	vec3 N = normalize(nWorld);

	vec3 colorSum = vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < numLights; i++)
	{
		vec3 L = normalize(Lights[i].direction);
		L = vec3(skyboxRotation * vec4(L, 0.0));
		vec3 I = Lights[i].color;
		vec3 H = normalize(L + V);

		float NdotL = max(dot(N, L), 0.0);
		float NdotH = max(dot(N, H), 0.0);

		vec3 diffuse = I * kd * NdotL;
		vec3 specular = I * ks * pow(NdotH, 50.0);

		colorSum += diffuse + specular * specularStatus;
	}
	
	vec3 direction = vec3(pWorld) - eyePos;
    direction = reflect(direction, nWorld);
    direction.x = -direction.x;

	vec4 mirrorColor = texture(skybox, direction);

	fragColor = vec4(colorSum, 1.0);

	fragColor = mix(fragColor, mirrorColor, 0.7);

	float delta = 1e-5;
	fragColor.r = log(fragColor.r + delta);
	fragColor.g = log(fragColor.g + delta);
	fragColor.b = log(fragColor.b + delta);

	//fragColor = mix(fragColor, mirrorColor, 0.8);
	//fragColor += mirrorColor * 0.5;

	fragColor.a = 1.0;
}
