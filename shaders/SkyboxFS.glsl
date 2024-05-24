#version 460 core
out vec4 FragColor;

in vec3 direction;

uniform samplerCube skybox;

uniform mat4 skyboxRotation;

void main()
{
    vec3 newDirection = vec3(skyboxRotation * vec4(direction, 0.0));

    FragColor = texture(skybox, newDirection);

    float delta = 1e-5;
    FragColor.r = log(FragColor.r + delta);
	FragColor.g = log(FragColor.g + delta);
	FragColor.b = log(FragColor.b + delta);

	FragColor.a = 1.0;
}
