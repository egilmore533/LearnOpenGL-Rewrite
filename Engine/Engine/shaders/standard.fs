#version 330 core
out vec4 frag_color;

in vec3 fragment_position;
in vec3 normal;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 view_position;

void main()
{
	//ambient calculation
	float ambient_strength = 0.1;
	vec3 ambient_component = ambient_strength * light_color;

	//diffuse calculation
	vec3 norm = normalize(normal);
	vec3 light_direction = normalize(light_position - fragment_position);
	float diffuse_impact = max(dot(norm, light_direction), 0.0);
	vec3 diffuse_component = diffuse_impact * light_color;

	//specular calculation
	float specular_strength = 0.5;
	vec3 view_direction = normalize(view_position - fragment_position);
	vec3 reflection_direction = reflect(-light_direction, norm);
	float specular_impact = pow(max(dot(view_direction, reflection_direction), 0.0), 32);
	vec3 specular_component = specular_strength * specular_impact * light_color;

	frag_color = vec4((ambient_component + diffuse_component + specular_component) * object_color, 1.0);
}
