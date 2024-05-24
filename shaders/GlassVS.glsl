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
    float ratio;
    
    //ratio = 1.0 / 1.33; // air to water
    ratio = 1.0 / 1.52; // air to glass
    //ratio = 1.0 / 2.42; // air to diamond
    
    direction = refract(direction, nWorld, ratio);
    direction.x = -direction.x;
    //direction = normalize(direction);
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(InVertex, 1);
}
