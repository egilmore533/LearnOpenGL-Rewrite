#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texture_coordinates;

layout (std140) uniform matrices
{
	mat4 projection;
	mat4 view;
};

out VS_OUT {
	vec3 frag_pos;
	vec3 normal;
	vec2 texture_coordinates;
} vs_out;

void main()
{
	vs_out.frag_pos = a_pos;
	vs_out.normal = a_normal;
	vs_out.texture_coordinates = a_texture_coordinates;
	gl_Position = projection * view * vec4(a_pos, 1.0);
}
