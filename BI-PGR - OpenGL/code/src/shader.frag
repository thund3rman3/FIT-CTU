/**
 * @file    shader.frag
 * @author  Josef Jech
 * @date    15.05.2023
 */

#version 140

struct Sun 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};

struct PointLight 
{     
    vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

struct SpotLight 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;      
	vec3 direction; 
	float cosCutOff; // cosine of the spotlight's half angle
	float exponent;  // distribution of the light energy within the reflector's cone (center->cone's edge)
};

struct Material 
{          
    vec3  ambient;            
    vec3  diffuse;        
    vec3  specular;          
    float shininess;          // sharpness of specular reflection
    bool  useTexture;         // defines whether the texture is used or not
};

uniform Sun sun;
PointLight pointLight;
uniform SpotLight spotLight;
uniform vec3 globalAmbientLight;

uniform Material material;     // current material
uniform sampler2D texSampler;  // sampler for the texture access
uniform float time;        
uniform mat4 Vmatrix;
uniform bool useFlashlight;
uniform vec3 cameraPosition;
uniform bool fog;
float fogDensity = 0.015;
vec4 fogColor = vec4 (0.86, 0.855, 0.865, 1.0);

smooth in vec2 texCoord_v;     // fragment texture coordinates
smooth in vec3 vertexPosition;
smooth in vec3 vertexNormal;

out vec4 color_f;        // outgoing fragment color

//reflector
vec4 calculateSpotLight(SpotLight spotLight, Material material, vec3 vertexPosition, vec3 vertexNormal) {

  vec3 ret = vec3(0.0);

  // use the material and light structures to obtain the surface and light properties
  // the vertexPosition and vertexNormal variables contain transformed surface position and normal
  // store the ambient, diffuse and specular terms to the ret variable
  // for spot lights, light.position contains the light position
  // everything is expressed in the view coordinate system -> eye/camera is in the origin

  vec3 L = normalize(spotLight.position - vertexPosition);
  vec3 R = reflect(-L, vertexNormal);
  vec3 V = normalize(-vertexPosition);

  float NdotL = max(0.0, dot(vertexNormal, L));
  float RdotV = max(0.0, dot(R, V));
  float spotCoef = max(0.0, dot(-L, spotLight.direction));

  ret += material.ambient * spotLight.ambient;
  ret += material.diffuse * spotLight.diffuse * NdotL;
  ret += material.specular * spotLight.specular * pow(RdotV, material.shininess);

  if(spotCoef < spotLight.cosCutOff)
    ret *= 0.0;
  else
    ret *= pow(spotCoef, spotLight.exponent);

  return vec4(ret, 1.0);
}

//sun
vec4 calculateDirectionalLight(Sun sun, Material material, vec3 vertexPosition, vec3 vertexNormal) {

  vec3 ret = vec3(0.0);

  // use the material and light structures to obtain the surface and light properties
  // the vertexPosition and vertexNormal variables contain transformed surface position and normal
  // store the ambient, diffuse and specular terms to the ret variable
  // glsl provides some built-in functions, for example: reflect, normalize, pow, dot
  // for directional lights, light.position contains the direction
  // everything is expressed in the view coordinate system -> eye/camera is in the origin

  vec3 L = normalize(sun.direction);
  vec3 R = reflect(-L, vertexNormal);
  vec3 V = normalize(-vertexPosition);
  float NdotL = max(0.0, dot(vertexNormal, L));
  float RdotV = max(0.0, dot(R, V));

  ret += material.ambient * sun.ambient;
  ret += material.diffuse * sun.diffuse * NdotL;
  ret += material.specular * sun.specular * pow(RdotV, material.shininess);

  return vec4(ret, 1.0);
}

//fire
vec4 calculatePointLight(PointLight pointLight, Material material, vec3 vertexPosition, vec3 vertexNormal)
{
    vec3 ret = vec3(0.0); 

    const float constantAttenuationCoeff = 0.0f;
    const float linearAttenuationCoeff = 0.02f;
    const float quadraticAttenuationCoeff = 0.0f;

    vec3 L = normalize(pointLight.position - vertexPosition);
    vec3 R = reflect(-L, vertexNormal);
    vec3 V = normalize(-vertexPosition);
    float NdotL = max(0.0, dot(vertexNormal, L));
    float RdotV = max(0.0, dot(R, V));
    
    ret += material.ambient * pointLight.ambient;
    ret += material.diffuse * pointLight.diffuse * NdotL;
    ret += material.specular * pointLight.specular * pow(RdotV, material.shininess);

    float dist = distance(pointLight.position, vertexPosition);

    float attenuationFactor = 1 / ( constantAttenuationCoeff + linearAttenuationCoeff * dist + quadraticAttenuationCoeff * pow ( dist, 2 ) ); 

    ret *= attenuationFactor;

    return vec4 (ret, 1.0);
}


float fogFactor ( vec3 fragPos, vec3 camPos ) {
    
    float dist = sqrt ( pow ( fragPos.x - camPos.x, 2 ) + pow ( fragPos.y - camPos.y, 2 ) + pow ( fragPos.z - camPos.z, 2 ) );

    return exp ( - pow (fogDensity * dist, 2) );
}

void main() 
{
    // initialize the output color with the global ambient term
    vec4 outputColor = vec4(material.ambient * globalAmbientLight, 0.0);
    vec3 normal = normalize ( vertexNormal );

    // campfire light setup
	pointLight.ambient =  vec3((sin(time*0.5f)+1.1f)/10, 0.0f ,0.0f);
	pointLight.diffuse =  vec3((sin(time*0.5f)+1.1f)/5, 0.0f ,0.0f);
	pointLight.specular = vec3((sin(time*0.5f)+1.1f)/5, 0.0f ,0.0f);
	pointLight.position = vec3(50.0, 50.0, -2.0);

    // accumulate contributions from all lights
    outputColor += calculateDirectionalLight(sun, material, vertexPosition, normal);
    outputColor += calculatePointLight(pointLight, material, vertexPosition, normal);
    vec4 spotLightEffect = vec4(0.0f);

    if (useFlashlight )
        spotLightEffect = calculateSpotLight(spotLight, material, vertexPosition, normal);

    outputColor += spotLightEffect; 

    // if material has a texture -> apply it
    if (material.useTexture) 
    {
        vec4  fragmentColor = outputColor * texture(texSampler, texCoord_v);

        if ( !fog ) 
            color_f = fragmentColor;
        else 
        {
            float f = fogFactor ( vertexPosition, cameraPosition );
            float fogChangeAmplitude = 0.1;
            float fogChangeSpeedCoeff = 0.5;
            fogColor += vec4 ( vec3( cos(time * fogChangeSpeedCoeff) * fogChangeAmplitude).xyz, 1.0f);
            color_f = fragmentColor * f + ( 1 - f ) * fogColor;
        }
    }
}
