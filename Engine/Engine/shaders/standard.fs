#version 330 core
out vec4 frag_color;

struct Material {
    sampler2D diffuse_map1;
	sampler2D specular_map1;
	sampler2D normal_map1;
	sampler2D height_map1;
	sampler2D emission_map1;
    float shininess;
}; 

struct Directional_Light {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

struct Point_Light {
    vec3 position;
    
    float attenuation_constant;
    float attenuation_linear;
    float attenuation_quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Spot_Light {
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

#define MAX_POINT_LIGHTS 4

in vec3 fragment_position;
in vec3 normal;
in vec2 texture_coordinates;
  
uniform Material material;
uniform Directional_Light directional_light;
uniform Point_Light point_lights[MAX_POINT_LIGHTS];
uniform Spot_Light spot_light;

vec4 calulate_directional_light(Directional_Light light, vec3 normal, vec3 view_direction);
vec4 calulate_point_light(Point_Light light, vec3 normal, vec3 fragment_position, vec3 view_direction);
vec4 calculate_spot_light(Spot_Light light, vec3 normal, vec3 fragment_position, vec3 view_direction);

void main()
{
	// calculate properties
    vec3 norm = normalize(normal);
    vec3 view_direction = normalize(-fragment_position);

	// phase 1: Directional lighting
    vec4 result = calulate_directional_light(directional_light, norm, view_direction);

	// phase 2: Point lights
    for(int i = 0; i < MAX_POINT_LIGHTS; i++)
        result += calulate_point_light(point_lights[i], norm, fragment_position, view_direction);    

	// phase 3: spot light
    result += calculate_spot_light(spot_light, norm, fragment_position, view_direction);

	frag_color = result;
}


vec4 calulate_directional_light(Directional_Light light, vec3 normal, vec3 view_direction)
{
	vec3 light_direction = normalize(-light.direction);
    // diffuse shading
    float diffuse_impact = max(dot(normal, light_direction), 0.0);
    // specular shading
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_impact = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);

    // combine results
    vec4 ambient_component  = vec4(light.ambient, 1.0)  * texture(material.diffuse_map1, texture_coordinates);
    vec4 diffuse_component  = vec4(light.diffuse, 1.0)  * diffuse_impact * texture(material.diffuse_map1, texture_coordinates);
    vec4 specular_component = vec4(light.specular, 1.0) * specular_impact * texture(material.specular_map1, texture_coordinates);

    return (ambient_component + diffuse_component + specular_component);
}

vec4 calulate_point_light(Point_Light light, vec3 normal, vec3 fragment_position, vec3 view_direction)
{
	vec3 light_direction = normalize(light.position - fragment_position);
    // diffuse shading
    float diffuse_impact = max(dot(normal, light_direction), 0.0);
    // specular shading
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_impact = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
	// attenuation calculation
	float distance    = length(light.position - fragment_position);
	float attenuation = 1.0 / (light.attenuation_constant + light.attenuation_linear * distance + light.attenuation_quadratic * (distance * distance)); 

    // combine results
    vec4 ambient_component  = vec4(light.ambient, 1.0)  * texture(material.diffuse_map1, texture_coordinates);
    vec4 diffuse_component  = vec4(light.diffuse, 1.0)  * diffuse_impact * texture(material.diffuse_map1, texture_coordinates);
    vec4 specular_component = vec4(light.specular, 1.0) * specular_impact * texture(material.specular_map1, texture_coordinates);

	ambient_component *= attenuation;
	diffuse_component *= attenuation;
	specular_component *= attenuation;

    return (ambient_component + diffuse_component + specular_component);
}

vec4 calculate_spot_light(Spot_Light light, vec3 normal, vec3 fragment_position, vec3 view_direction)
{
    vec3 light_direction = normalize(light.position - fragment_position);
    // diffuse shading
    float diffuse_impact = max(dot(normal, light_direction), 0.0);
    // specular shading
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_impact = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
	// attenuation calculation
	float distance    = length(light.position - fragment_position);
	float attenuation = 1.0 / (light.attenuation_constant + light.attenuation_linear * distance + light.attenuation_quadratic * (distance * distance)); 
	// spotlight intensity and soft edge calculation
	float theta		= dot(light_direction, normalize(-light.direction));
	float epsilon	= light.cut_off - light.outer_cut_off;
	float intensity	= clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

	// combine results
    vec4 ambient_component  = vec4(light.ambient, 1.0)  * texture(material.diffuse_map1, texture_coordinates);
    vec4 diffuse_component  = vec4(light.diffuse, 1.0)  * diffuse_impact * texture(material.diffuse_map1, texture_coordinates);
    vec4 specular_component = vec4(light.specular, 1.0) * specular_impact * texture(material.specular_map1, texture_coordinates);

	ambient_component	*= attenuation * intensity;
	diffuse_component	*= attenuation * intensity;
	specular_component	*= attenuation * intensity;

    return (ambient_component + diffuse_component + specular_component);
}
