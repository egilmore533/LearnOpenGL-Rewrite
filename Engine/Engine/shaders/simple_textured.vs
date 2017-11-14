#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texture_coordinates;

layout (std140) uniform matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

out vec2 texture_coordinates;

void main()
{
	texture_coordinates = a_texture_coordinates;
	gl_Position = projection * view * model * vec4(a_pos, 1.0f);
}