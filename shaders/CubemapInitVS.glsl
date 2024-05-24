#version 460 core

layout (location = 0) in vec3 InVertex;

out vec3 direction;

uniform mat4 model;
uniform mat4 rotation;
uniform mat4 projection;
uniform mat4 view;

void main() {
    direction = (rotation * model * vec4(InVertex, 1.0)).xyz;
    direction = normalize(direction);
    gl_Position = projection * view * model * vec4(InVertex, 1.0);
}
