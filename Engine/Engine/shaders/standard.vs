#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

out vec3 fragment_position;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

void main()
{
	fragment_position = vec3(view * model * vec4(a_position, 1.0));
	normal = normal_matrix * a_normal;

	gl_Position = projection * view * model * vec4(a_position, 1.0);
}
