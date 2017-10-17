#version 330 core
out vec4 frag_color;

in vec2 texture_coordinates;

uniform sampler2D screen_texture;

const float offset = 1.0 / 300; // this is how far each sample from the texture will be from the current pixel

void main()
{
	// these are the offsets we will use to sample the texture from different locations areound the current texture coordinate
	// these correspond to our kernel's values
	vec2 offsets[9] = vec2[](
		vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right 
	);

	// this is an array of values centered on the current pixel to multiply the surrounding pixel values by
	float kernel[9] = float[](
		-1, -1, -1,
		-1, 9, -1,
		-1, -1, -1
	);

	// this array is populated by the color values from the texture at the corresponding positions from the offsets array
	vec3 sample_from_texture[9];
	for(int i = 0; i < 9; i++)
	{
		sample_from_texture[i] = vec3(texture(screen_texture, texture_coordinates.st + offsets[i]));
	}

	// this total color will be created by adding to it the product of the kernel and the sample from the texture
	vec3 total_color = vec3(0.0);
	for(int i = 0; i < 9; i++)
	{
		total_color += sample_from_texture[i] * kernel[i];
	}

	frag_color = vec4(total_color, 1.0);
}