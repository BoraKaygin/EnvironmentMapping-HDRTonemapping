#version 460 core
layout(location = 0) in vec3 InVertex;

out vec2 TexCoords;

void main()
{
    TexCoords = InVertex.xy;
    TexCoords += 1.0;
    TexCoords /= 2.0;
    gl_Position = vec4(InVertex.x, InVertex.y, 0.0, 1.0);
}