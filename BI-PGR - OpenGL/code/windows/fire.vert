/**
 * @file    fire.vert
 * @author  Josef Jech
 * @date    15.05.2023
 */

#version 140

// projection view model matrix
uniform mat4 PVMmatrix; 
// vertex position in
in vec3 position; 
// texture coordinates in
in vec2 texCoord;

// texture coordinates out
smooth out vec2 texCoord_v; 

void main() 
{
	gl_Position = PVMmatrix * vec4(position, 1);

	// send coords to the fragment shader
	texCoord_v = texCoord; 
}