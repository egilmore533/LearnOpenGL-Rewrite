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
uniform bool gamma_enabled;

vec3 blinn_phong(vec3 normal, vec3 frag_pos, vec3 light_position, vec3 light_color)
{
	// diffuse
	vec3 light_direction = normalize(light_position - fs_in.frag_pos);
	float diffuse_impact = max(dot(light_direction, normal), 0.0);
	vec3 diffuse_component = diffuse_impact * light_color;

	// specular 
	vec3 view_direction = normalize(view_position - frag_pos);
	vec3 reflect_direction = reflect(-light_direction, normal);
	vec3 halfway_direction = normalize(light_direction + view_direction);
	float specular_impact = pow(max(dot(normal, halfway_direction), 0.0), 32.0);
	vec3 specular_component = specular_impact * light_color;

	// simple attentuation
	float max_distance = 1.5;
	float distance = length(light_position - frag_pos);
	float attentuation = 1.0 / (gamma_enabled ? distance * distance : distance);

	diffuse_component *= attentuation;
	specular_component *= attentuation;

	return diffuse_component + specular_component;
}

void main()
{
	vec3 color = texture(floor_texture, fs_in.texture_coordinates).rgb;

	vec3 lighting = vec3(0.0);

	lighting += blinn_phong(normalize(fs_in.normal), fs_in.frag_pos, light_position, vec3(1.0));
	
	color *= lighting;
	
	if(gamma_enabled)
		color = pow(color, vec3(1.0/2.2));

	frag_color = vec4(color, 1.0);
}
