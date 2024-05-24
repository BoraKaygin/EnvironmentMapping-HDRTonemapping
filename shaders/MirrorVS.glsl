#version 460 core
layout (location = 0) in vec3 InVertex;
layout (location = 1) in vec3 InNormal;

out vec3 direction;

uniform mat4 modelingMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewingMatrix;
uniform vec3 eyePos;

void main()
{   
    vec4 pWorld = modelingMatrix * vec4(InVertex, 1);
	vec3 nWorld = inverse(transpose(mat3x3(modelingMatrix))) * InNormal;

    direction = vec3(pWorld) - eyePos;
    direction = reflect(direction, nWorld);
    direction.x = -direction.x;
    //direction = normalize(direction);
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(InVertex, 1);
}
