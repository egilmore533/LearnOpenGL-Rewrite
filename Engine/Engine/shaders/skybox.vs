#version 330 core
layout (location = 0) in vec3 a_pos;

out vec3 texture_coordinates;

layout (std140) uniform matrices
{
	mat4 projection;
	mat4 view;
};

// uniform mat4 projection;
uniform mat4 view_no_translation;

void main()
{
	texture_coordinates = a_pos;
	vec4 temp_pos = projection * view_no_translation * vec4(a_pos, 1.0);
	gl_Position = temp_pos.xyww;
}
