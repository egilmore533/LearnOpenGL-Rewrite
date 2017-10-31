#version 330 core
out vec4 frag_color;

in vec3 normal;
in vec3 position;

uniform vec3 camera_pos;
uniform samplerCube skybox;

void main()
{
	float ratio = 1.00 / 1.52;
	vec3 view_direction = normalize(position - camera_pos);
	vec3 refraction = refract(view_direction, normalize(normal), ratio);
	frag_color = vec4(texture(skybox, refraction).rgb, 1.0);
}
