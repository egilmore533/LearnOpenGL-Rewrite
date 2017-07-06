#version 330 core
out vec4 frag_color;

uniform vec3 lamp_color;

void main()
{
    frag_color = vec4(lamp_color, 1.0); 
}