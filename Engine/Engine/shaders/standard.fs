#version 330 core
out vec4 frag_color;

struct Material {
    sampler2D m_diffuse_map;
	sampler2D m_specular_map;
	sampler2D m_emission_map;
    float m_shininess;
}; 

struct Directional_Light {
    vec3 m_direction;
  
    vec3 m_ambient;	//refers to the m_color of the m_ambient component of this directional light
    vec3 m_diffuse;	//refers to the m_color of the m_diffuse component of this directional light
    vec3 m_specular;	//refers to the m_color of the m_specular component of this directional light

	vec3 m_color;
}; 

struct Point_Light {
    vec3 m_position;
    
    float m_attenuation_constant;
    float m_attenuation_linear;
    float m_attenuation_quadratic;
	
    vec3 m_ambient;
    vec3 m_diffuse;
    vec3 m_specular;

	vec3 m_color;
};

struct Spot_Light {
    vec3 m_position;
	vec3 m_direction;
	float m_cut_off;
    float m_outer_cut_off;

    vec3 m_ambient;
    vec3 m_diffuse;
    vec3 m_specular;

	float m_attenuation_constant;
    float m_attenuation_linear;
    float m_attenuation_quadratic;
};

#define MAX_POINT_LIGHTS 4

in vec3 fragment_position;
in vec3 normal;
in vec2 texture_coordinates;
  
uniform Material material;
uniform Directional_Light directional_light;
uniform Point_Light point_lights[MAX_POINT_LIGHTS];
uniform Spot_Light spot_light;

vec3 calulate_directional_light(Directional_Light light, vec3 normal, vec3 view_direction);
vec3 calulate_point_light(Point_Light light, vec3 normal, vec3 fragment_position, vec3 view_direction);
vec3 calculate_spot_light(Spot_Light light, vec3 normal, vec3 fragment_position, vec3 view_direction);

void main()
{
	// calculate properties
    vec3 norm = normalize(normal);
    vec3 view_direction = normalize(-fragment_position);

	// phase 1: Directional lighting
    vec3 result = calulate_directional_light(directional_light, norm, view_direction);

	// phase 2: Point lights
    for(int i = 0; i < MAX_POINT_LIGHTS; i++)
        result += calulate_point_light(point_lights[i], norm, fragment_position, view_direction);    

	// phase 3: spot light
    result += calculate_spot_light(spot_light, norm, fragment_position, view_direction);
	
	frag_color = vec4(result, 1.0);
}


vec3 calulate_directional_light(Directional_Light light, vec3 normal, vec3 view_direction)
{
	vec3 light_direction = normalize(-light.m_direction);
    // m_diffuse shading
    float diffuse_impact = max(dot(normal, light_direction), 0.0);
    // m_specular shading
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_impact = pow(max(dot(view_direction, reflect_direction), 0.0), material.m_shininess);

    // combine results
    vec3 ambient_component  = light.m_ambient  * vec3(texture(material.m_diffuse_map, texture_coordinates));
    vec3 diffuse_component  = light.m_diffuse  * diffuse_impact * vec3(texture(material.m_diffuse_map, texture_coordinates));
    vec3 specular_component = light.m_specular * specular_impact * vec3(texture(material.m_specular_map, texture_coordinates));

    return (ambient_component + diffuse_component + specular_component);
}

vec3 calulate_point_light(Point_Light light, vec3 normal, vec3 fragment_position, vec3 view_direction)
{
	vec3 light_direction = normalize(light.m_position - fragment_position);
    // m_diffuse shading
    float diffuse_impact = max(dot(normal, light_direction), 0.0);
    // m_specular shading
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_impact = pow(max(dot(view_direction, reflect_direction), 0.0), material.m_shininess);
	// attenuation calculation
	float distance    = length(light.m_position - fragment_position);
	float attenuation = 1.0 / (light.m_attenuation_constant + light.m_attenuation_linear * distance + light.m_attenuation_quadratic * (distance * distance)); 

    // combine results
    vec3 ambient_component  = light.m_ambient  * vec3(texture(material.m_diffuse_map, texture_coordinates));
    vec3 diffuse_component  = light.m_diffuse  * diffuse_impact * vec3(texture(material.m_diffuse_map, texture_coordinates));
    vec3 specular_component = light.m_specular * specular_impact * vec3(texture(material.m_specular_map, texture_coordinates));

	ambient_component *= attenuation;
	diffuse_component *= attenuation;
	specular_component *= attenuation;

    return (ambient_component + diffuse_component + specular_component);
}

vec3 calculate_spot_light(Spot_Light light, vec3 normal, vec3 fragment_position, vec3 view_direction)
{
    vec3 light_direction = normalize(light.m_position - fragment_position);
    // m_diffuse shading
    float diffuse_impact = max(dot(normal, light_direction), 0.0);
    // m_specular shading
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_impact = pow(max(dot(view_direction, reflect_direction), 0.0), material.m_shininess);
	// attenuation calculation
	float distance    = length(light.m_position - fragment_position);
	float attenuation = 1.0 / (light.m_attenuation_constant + light.m_attenuation_linear * distance + light.m_attenuation_quadratic * (distance * distance)); 
	//spotlight intensity and soft edge calculation
	float theta		= dot(light_direction, normalize(-light.m_direction));
	float epsilon	= light.m_cut_off - light.m_outer_cut_off;
	float intensity	= clamp((theta - light.m_outer_cut_off) / epsilon, 0.0, 1.0);

	// combine results
    vec3 ambient_component  = light.m_ambient  * vec3(texture(material.m_diffuse_map, texture_coordinates));
    vec3 diffuse_component  = light.m_diffuse  * diffuse_impact * vec3(texture(material.m_diffuse_map, texture_coordinates));
    vec3 specular_component = light.m_specular * specular_impact * vec3(texture(material.m_specular_map, texture_coordinates));

	ambient_component	*= attenuation * intensity;
	diffuse_component	*= attenuation * intensity;
	specular_component	*= attenuation * intensity;

    return (ambient_component + diffuse_component + specular_component);
}
