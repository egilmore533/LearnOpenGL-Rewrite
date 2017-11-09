#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 2) in vec2 a_tex_coords;

layout (std140) uniform matrices
{
	mat4 projection;
	mat4 view;
};

out VS_OUT {
    vec2 tex_coords;
} vs_out;

uniform mat4 model;

void main()
{
    vs_out.tex_coords = a_tex_coords;
    gl_Position = projection * view * model * vec4(a_position, 1.0); 
}