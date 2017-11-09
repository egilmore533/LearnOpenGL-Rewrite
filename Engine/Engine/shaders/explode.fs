#version 330 core
out vec4 frag_color;

in vec2 texture_coordinates;

uniform sampler2D texture_diffuse;

void main()
{
    frag_color = texture(texture_diffuse, texture_coordinates);
}