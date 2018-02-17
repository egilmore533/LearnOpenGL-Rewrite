#version 330 core
out vec4 frag_color;

in VS_OUT 
{
    vec3 fragment_position;
    vec3 normal;
    vec2 texture_coordinates;
} fs_in;

uniform sampler2D diffuse_texture;
uniform samplerCube depth_cube_map;

uniform vec3 light_position;
uniform vec3 view_position;

uniform float far_plane;
uniform bool shadows;

float shadow_calculation(vec3 frag_position)
{
	// get direction from frag_position to light_position
	vec3 frag_to_light = frag_position - light_position;
	// now get current linear depth as the length between the fragment and the light_position
	float current_depth = length(frag_to_light);
	// take samples from around the fragment position and average the results for a smooth shadow
	float shadow = 0.0;
	float bias = 0.05;
	float samples = 4.0;
	float offset = 0.1;
	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
		for(float y = -offset; y < offset; y += offset / (samples * 0.5))
		{
			for(float z = -offset; z < offset; z += offset / (samples * 0.5))
			{
				float closest_depth = texture(depth_cube_map, frag_to_light + vec3(x,y,z)).r;
				closest_depth *= far_plane;
				if(current_depth - bias > closest_depth)
					shadow += 1.0;
			}
		}
	}

	shadow /= (samples * samples * samples);
	
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
	float shadow = shadows ? shadow_calculation(fs_in.fragment_position) : 0.0;
	vec3 lighting = (ambient_component + (1.0 - shadow) * (diffuse_component + specular_component)) * color;

	frag_color = vec4(lighting, 1.0);
	
	// this will visualize the depth_cube_map for debugging
	//vec3 frag_to_light = fs_in.fragment_position - light_position;
	//float closest_depth = texture(depth_cube_map, frag_to_light).r;
	//closest_depth *= far_plane;
	//frag_color = vec4(vec3(closest_depth / far_plane), 1.0);  
}