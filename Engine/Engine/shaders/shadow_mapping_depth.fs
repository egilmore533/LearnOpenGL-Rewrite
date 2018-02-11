#version 330 core

void main()
{             
	// we don't need to actually type this but we could if we wanted to
    gl_FragDepth = gl_FragCoord.z; 
}