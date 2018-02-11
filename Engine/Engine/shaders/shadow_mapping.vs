#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texture_coordinates;

out VS_OUT 
{
	vec3 fragment_position;
	vec3 normal;
	vec2 texture_coordinates;
	vec4 fragment_position_light_space;
} vs_out;

layout (std140) uniform matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;
uniform mat4 light_space_matrix;

void main()
{
	vs_out.fragment_position = vec3(model * vec4(a_position, 1.0));
	vs_out.normal = transpose(inverse(mat3(model))) * a_normal;
	vs_out.texture_coordinates = a_texture_coordinates;
	vs_out.fragment_position_light_space = light_space_matrix * vec4(vs_out.fragment_position, 1.0);
	gl_Position = projection * view * model * vec4(a_position, 1.0);
}
