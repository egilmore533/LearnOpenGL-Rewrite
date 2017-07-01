#version 330 core
out vec4 frag_color;

struct Material {
    sampler2D diffuse_map;
	sampler2D specular_map;
	sampler2D emission_map;
    float shininess;
}; 

struct Light {
    vec3 position;
	vec3 direction;
	float cut_off;
    float outer_cut_off;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float attenuation_constant;
    float attenuation_linear;
    float attenuation_quadratic;
};

in vec3 fragment_position;
in vec3 normal;
in vec2 texture_coordinates;
  
uniform Material material;
uniform Light light;
//uniform vec3 view_position;		we do calculations in view space so this isn't needed'

void main()
{
	vec3 light_direction = normalize(light.position - fragment_position);
	float theta = dot(light_direction, normalize(-light.direction));

	if(theta > light.cut_off)
	{
		//ambient calculation
		vec3 ambient_component = light.ambient * texture(material.diffuse_map, texture_coordinates).rgb;

		//diffuse calculation
		vec3 norm = normalize(normal);
		float diffuse_impact = max(dot(norm, light_direction), 0.0);
		vec3 diffuse_component = light.diffuse * diffuse_impact * texture(material.diffuse_map, texture_coordinates).rgb;

		//specular calculation
		vec3 view_direction = normalize(-fragment_position); // the viewer is always at (0,0,0) in view-space, so view_direction is (0,0,0) - Position => -Position
		vec3 reflection_direction = reflect(-light_direction, norm);
		float specular_impact = pow(max(dot(view_direction, reflection_direction), 0.0), material.shininess);
		vec3 specular_component = light.specular * specular_impact * texture(material.specular_map, texture_coordinates).rgb;

		// emission calculation
		//vec3 emission_component = texture(material.emission_map, texture_coordinates).rgb;

		// attenuation calculation
		float distance    = length(light.position - fragment_position);
		float attenuation = 1.0 / (light.attenuation_constant + light.attenuation_linear * distance + light.attenuation_quadratic * (distance * distance));    

		//ambient_component  *= attenuation;  
		diffuse_component  *= attenuation;
		specular_component *= attenuation;  

		vec3 result = ambient_component + diffuse_component + specular_component;
		frag_color = vec4(result, 1.0);
	}
	else 
    {
        // else, use ambient light so scene isn't completely dark outside the spotlight.
        frag_color = vec4(light.ambient * texture(material.diffuse_map, texture_coordinates).rgb, 1.0);
    }	
}
