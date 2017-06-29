#version 330 core
out vec4 frag_color;

struct Material {
    sampler2D diffuse_map;
	sampler2D specular_map;
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 fragment_position;
in vec3 normal;
in vec2 texture_coordinates;
  
uniform Material material;
uniform Light light;
uniform vec3 view_position;

void main()
{
	//ambient calculation
	vec3 ambient_component = light.ambient * texture(material.diffuse_map, texture_coordinates).rgb;

	//diffuse calculation
	vec3 norm = normalize(normal);
	vec3 light_direction = normalize(light.position - fragment_position);
	float diffuse_impact = max(dot(norm, light_direction), 0.0);
	vec3 diffuse_component = light.diffuse * diffuse_impact * texture(material.diffuse_map, texture_coordinates).rgb;

	//specular calculation
	vec3 view_direction = normalize(-fragment_position); // the viewer is always at (0,0,0) in view-space, so view_direction is (0,0,0) - Position => -Position
	vec3 reflection_direction = reflect(-light_direction, norm);
	float specular_impact = pow(max(dot(view_direction, reflection_direction), 0.0), material.shininess);
	vec3 specular_component = light.specular * specular_impact * texture(material.specular_map, texture_coordinates).rgb;

	vec3 result = ambient_component + diffuse_component + specular_component;
	frag_color = vec4(result, 1.0);
}
