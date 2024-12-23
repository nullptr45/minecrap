#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex_coords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(pos, 1.f);
    TexCoords = tex_coords;
}
