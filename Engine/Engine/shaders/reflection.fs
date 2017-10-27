#version 330 core
out vec4 frag_color;

in vec3 normal;
in vec3 position;

uniform vec3 camera_pos;
uniform samplerCube skybox;

void main()
{
	vec3 view_direction = normalize(position - camera_pos);
	vec3 reflection = reflect(view_direction, normalize(normal));
	frag_color = vec4(texture(skybox, reflection).rgb, 1.0);
}
