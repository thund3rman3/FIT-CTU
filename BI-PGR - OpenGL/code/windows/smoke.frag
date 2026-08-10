/**
 * @file    smoke.frag
 * @author  Josef Jech
 * @date    15.05.2023
 */

#version 140

// sampler for texture access
uniform sampler2D texSampler; 
// fragment texture coordinates
smooth in vec2 texCoord_v;    
// outgoing fragment color
out vec4 color_f;             

void main() {
    color_f = texture(texSampler, texCoord_v);
}