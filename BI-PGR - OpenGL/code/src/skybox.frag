/**
 * @file    skybox.frag
 * @author  Josef Jech
 * @date    15.05.2023
 */

#version 140
  
uniform samplerCube skyboxSampler;
uniform float time;
in vec3 texCoord_v;
out vec4 color_f;
  
uniform bool fog;
vec4 fogColor = vec4 (0.86, 0.855, 0.865, 1.0);

void main() 
{
	if (!fog)
		color_f = texture(skyboxSampler, texCoord_v);
	else {
		float fogChangeAmplitude = 0.1;
        float fogChangeSpeedCoeff = 0.5;
        fogColor += vec4 ( vec3( cos(time * fogChangeSpeedCoeff) * fogChangeAmplitude).xyz, 1.0f);
		color_f = fogColor;
	}
}