#version 330 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec3 a_color;

out vec3 f_color;

uniform vec2 offsets[100];

void main()
{
	// When drawing via one of the instanced rendering calls, 
	// gl_InstanceID is incremented for each instance being rendered starting from 0.
	vec2 offset = offsets[gl_InstanceID];
	gl_Position = vec4(a_pos + offset, 0.0, 1.0);
	f_color = a_color;
}