#version 330 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec2 a_offset; // define as an instanced array so the shader only updates the contents per instance rather than per vertex

out vec3 f_color;

void main()
{
	vec2 pos = a_pos * (gl_InstanceID / 100.0);
	gl_Position = vec4(pos + a_offset, 0.0, 1.0);
	f_color = a_color;
}