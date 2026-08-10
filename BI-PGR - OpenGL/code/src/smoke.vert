/**
 * @file    smoke.vert
 * @author  Josef Jech
 * @date    15.05.2023
 */

#version 140

uniform float time;
uniform mat4 PVMmatrix;    
in vec3 position;           
in vec2 texCoord;           
smooth out vec2 texCoord_v; 

void main() 
{
    // vertex position after the projection
    gl_Position = PVMmatrix * vec4(position, 1);
    float speed = 0.5f;
    vec2 textureStep = vec2(0.0f, -time * speed);

    // outputs entering the fragment shader
    texCoord_v = texCoord + textureStep;
}
