#version 330 core
out vec4 frag_color;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
  
uniform Material material;
uniform Light light;
uniform vec3 view_position;

void main()
{
	//ambient calculation
	vec3 ambient_component = light.ambient * material.ambient;

	//diffuse calculation
	vec3 norm = normalize(normal);
	vec3 light_direction = normalize(light.position - fragment_position);
	float diffuse_impact = max(dot(norm, light_direction), 0.0);
	vec3 diffuse_component = light.diffuse * (diffuse_impact * material.diffuse);

	//specular calculation
	vec3 view_direction = normalize(-fragment_position); // the viewer is always at (0,0,0) in view-space, so view_direction is (0,0,0) - Position => -Position
	vec3 reflection_direction = reflect(-light_direction, norm);
	float specular_impact = pow(max(dot(view_direction, reflection_direction), 0.0), material.shininess);
	vec3 specular_component = light.specular * (specular_impact * material.specular);

	vec3 result = ambient_component + diffuse_component + specular_component;
	frag_color = vec4(result, 1.0);
}
