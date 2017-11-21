#version 330 core
out vec4 frag_color;

in VS_OUT {
	vec3 frag_pos;
	vec3 normal;
	vec2 texture_coordinates;
} fs_in;

uniform sampler2D floor_texture;
uniform vec3 light_position;
uniform vec3 view_position;

void main()
{
	vec3 color = texture(floor_texture, fs_in.texture_coordinates).rgb;

	// ambient
	vec3 ambient_component = 0.05 * color;

	// diffuse
	vec3 light_direction = normalize(light_position - fs_in.frag_pos);
	vec3 normal = normalize(fs_in.normal);
	float diffuse_impact = max(dot(light_direction, normal), 0.0);
	vec3 diffuse_component = diffuse_impact * color;

	//specular
	vec3 view_direction = normalize(view_position - fs_in.frag_pos);
	vec3 reflect_direction = reflect(-light_direction, normal);
	vec3 halfway_direction = normalize(light_direction + view_direction);
	float specular_impact = pow(max(dot(normal, halfway_direction), 0.0), 32.0);
	vec3 specular_component = vec3(0.3) * specular_impact; // pure white light color

	frag_color = vec4(ambient_component + diffuse_component + specular_component, 1.0);
}
