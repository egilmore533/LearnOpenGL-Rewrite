#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

layout (std140) uniform matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

out VS_OUT 
{
	vec3 normal;
} vs_out;

void main()
{
	gl_Position = projection * view * model * vec4(a_pos, 1.0);
	mat3 normal_matrix = mat3(transpose(inverse(view * model)));
	vs_out.normal = normalize(vec3(projection * vec4(normal_matrix * a_normal, 1.0)));
}