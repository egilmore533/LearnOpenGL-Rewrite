#version 330 core
out vec4 frag_color;

in vec3 f_color;

void main()
{
	frag_color = vec4(f_color, 1.0);
}