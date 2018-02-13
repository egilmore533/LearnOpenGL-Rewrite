#version 330 core
out vec4 frag_color;

in VS_OUT 
{
	vec3 fragment_position;
	vec3 normal;
	vec2 texture_coordinates;
	vec4 fragment_position_light_space;
} fs_in;

uniform sampler2D diffuse_texture;
uniform sampler2D shadow_map;

uniform vec3 light_position;
uniform vec3 view_position;

float shadow_calculation(vec4 fragment_position_light_space, vec3 light_direction, vec3 normal)
{
	// perform perspective divide
    vec3 projection_coordinates = fragment_position_light_space.xyz / fragment_position_light_space.w;
    // transform to [0,1] range
    projection_coordinates = projection_coordinates * 0.5 + 0.5;

	if(projection_coordinates.z > 1.0)
		return 0.0;

    // get depth of current fragment from light's perspective
    float current_depth = projection_coordinates.z;
    // check whether current frag pos is in shadow

	// bias calculation to remove some shadow acne based on the surface angle towards the light
	float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005);  
	
	// sample more than once from the depth map, each time with a slightly different coordinate and combine each of the results for an average to get a softer shadow.
	float shadow = 0.0;
	vec2 texel_size = 1.0 / textureSize(shadow_map, 0);
	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			float pcf_depth = texture(shadow_map, projection_coordinates.xy + vec2(x, y) * texel_size).r;
			shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
		}
	}

    shadow /= 9.0;

	return shadow;
}

void main()
{
	vec3 color = texture(diffuse_texture, fs_in.texture_coordinates).rgb;
	vec3 normal = normalize(fs_in.normal);
	vec3 light_color = vec3(0.3);

	// ambient component
	vec3 ambient_component = 0.3 * color;

	// diffuse component
	vec3 light_direction = normalize(light_position - fs_in.fragment_position);
	float diff = max(dot(light_direction, normal), 0.0);
	vec3 diffuse_component = diff * light_color;

	// specular component
	vec3 view_direction = normalize(view_position - fs_in.fragment_position);
	vec3 reflect_direction = reflect(-light_direction, normal);
	float spec = 0.0f;
	vec3 halfway_direction = normalize(light_direction + view_direction);
	spec = pow(max(dot(normal, halfway_direction), 0.0), 64.0);
	vec3 specular_component = spec * light_color;

	// calculate shadow
	float shadow = shadow_calculation(fs_in.fragment_position_light_space, light_direction, fs_in.normal);
	vec3 lighting = (ambient_component + (1.0 - shadow) * (diffuse_component + specular_component)) * color;

	frag_color = vec4(lighting, 1.0);
}
