#version 330 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_texture_coordinates;

out vec2 texture_coordinates;

void main()
{
    gl_Position = vec4(a_pos.x, a_pos.y, 0.0, 1.0); 
    texture_coordinates = a_texture_coordinates;
}  