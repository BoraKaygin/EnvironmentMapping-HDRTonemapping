#version 460 core
layout (location = 0) in vec3 InVertex;
layout (location = 1) in vec3 InNormal;

out vec3 direction;

uniform mat4 projection;
uniform mat4 view;

void main()
{   
    direction = InVertex;
    direction.x = -direction.x;
    direction = normalize(direction);
    gl_Position = projection * view * vec4(InVertex, 1.0);
}
