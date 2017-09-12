#version 330 core
out vec4 frag_color;

in vec3 fragment_position;
in vec3 normal;
in vec2 texture_coordinates;

uniform sampler2D texture1;

void main()
{
    vec4 tex_color = texture(texture1, texture_coordinates);
    
    frag_color = tex_color;
}