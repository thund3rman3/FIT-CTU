/**
 * @file    fire.frag
 * @author  Josef Jech
 * @date    15.05.2023
 */

#version 140

// for selecting the proper animation frame
uniform float time; 
// sampler for the texture access
uniform sampler2D texSampler;
// fragment position in
smooth in vec3 position_v;  
// texture coordinates from the vertex shader in
smooth in vec2 texCoord_v;
// fragment color out
out vec4 color_f;	

 //4 frames in 4 rows
ivec2 pattern = ivec2(4, 4);
// how long will 1 frame last (in seconds)
float frameDuration = 0.1f; 

vec4 sampleTexture(int frame) 
{
	vec2 offset = vec2(1.0f, 1.0f);
	offset.x = offset.x/vec2(pattern).x;
	offset.y = offset.y / vec2(pattern).y;
	vec2 texCoordB = texCoord_v / vec2(pattern);
	vec2 texCoord = texCoordB + offset*vec2(frame % pattern.x, (frame / pattern.x));
	return texture(texSampler, texCoord);
}

void main()
{
	// frame from the texture
	int frame = int(time / frameDuration); 
	// outgoing fragment color 
	color_f = sampleTexture(frame);   
}
