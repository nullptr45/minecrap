#version 330 core

in vec3 UV;

out vec4 FragColor;

uniform sampler2DArray textures;

void main()
{
    FragColor = texture(textures, UV);
}
