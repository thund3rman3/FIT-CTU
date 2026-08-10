/**
 * @file    shader.vert
 * @author  Josef Jech
 * @date    15.05.2023
 */

#version 140


uniform mat4 PVMmatrix;     // Projection * View * Model
uniform mat4 Vmatrix;       // View                      
uniform mat4 Mmatrix;       // Model                      
uniform mat4 normalMatrix;  // inverse transposed Mmatrix

in vec3 position;           // vertex position in
in vec3 normal;             // vertex normal in
in vec2 texCoord;           // texture coordinates in

smooth out vec2 texCoord_v;  // texture coordinates out
smooth out vec3 vertexPosition; // vertex position in
smooth out vec3 vertexNormal;	// vertex normal out


void main() 
{
  // vertex position after the projection
  gl_Position = PVMmatrix * vec4(position, 1);

  // outputs entering the fragment shader
  texCoord_v = texCoord;

  // eye-coordinates position and normal of vertex
  vertexPosition = (Mmatrix * vec4(position, 1.0)).xyz;       
  vertexNormal   = normalize( (normalMatrix * vec4(normal, 0.0) ).xyz);
}