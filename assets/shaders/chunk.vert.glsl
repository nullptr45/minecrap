#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;

out vec2 UV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    UV = a_uv;
    gl_Position = proj * view * model * vec4(a_pos, 1.f);
}
