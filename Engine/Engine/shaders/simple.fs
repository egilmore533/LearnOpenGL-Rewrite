#version 330 core
out vec4 frag_color;
  
in vec2 texture_coordinates;

uniform sampler2D screen_texture;

void main()
{ 
	float gamma = 2.2;
    frag_color = texture(screen_texture, texture_coordinates);
	frag_color.rgb = pow(frag_color.rgb, vec3(1.0/gamma));
}