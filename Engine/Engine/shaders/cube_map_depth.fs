#version 330 core
in vec4 frag_position;

uniform vec3 light_position;
uniform float far_plane;

void main()
{
	float light_distance = length(frag_position.xyz - light_position);

	// map to [0;1] range by dividing by the far_plane
	light_distance = light_distance / far_plane;

	// write this for the modified depth
	gl_FragDepth = light_distance;
}
