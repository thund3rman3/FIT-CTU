precision highp float;

uniform float scale;
uniform float furScale;

// ---- Point light uniforms ----------------------------------------------
uniform vec3  uLightPos;       // light position in world space
uniform vec3  uLightColor;     // RGB color of the light
uniform float uLightIntensity; // light intensity (scales emitted power)
uniform float uAmbient;        // ambient light scale (approximates indirect lighting)
uniform bool  uLightEnabled;   // toggle direct lighting on/off

uniform sampler2D diffuseMap;  

in vec2 vUv;
in vec3 vWorldPos;
in vec3 vWorldNormal;
in float vLayer;
in vec4 vWorldTangent; // surface tangent in world space (U direction)

out vec4 fragColor;

const float PI = 3.14159265358979;
 
float hash(vec2 x) {
    return fract(sin(dot(x, vec2(127.1, 311.7))) * 43758.5453);
}

float valueNoise(vec2 uv) {
    vec2 i = floor(uv);
    vec2 f = fract(uv);  
    
    vec2 u = f * f * (3.0 - 2.0 * f);
    
    float a = hash(i + vec2(0.0, 0.0));
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

//--- BRDF helpers ------------------------------------------------------

    //https://graphicscompendium.com/theory/08-cook-torrance-ggx
    //https://graphicrants.blogspot.com/search?q=specular+brdf
    float NDF_GGX_D(float roughness, float NdotH){ 
        float a = (roughness) * (roughness); // smoothness in UE
        float num = a*a; 

        float term = NdotH * NdotH * (a*a - 1.0) + 1.0;
        float denom = PI * term * term;
        return num / denom; // GGX 
    }

    float F0_approx(float IOR){
        float term = (IOR - 1.0) / (IOR + 1.0);
        return term * term;
    }

    vec3 schlick_approx(float VdotH, float IOR, float spec){
        //spec = 1.0;
        vec3 F0 = vec3(F0_approx(IOR) * spec);
        return F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
    }

    float MNDF_Smith_GGX(float vec, float r){
        float denom = vec + sqrt(r*r + (1.0 - r*r) * vec*vec);
        return 1.0 / denom; // 2 * vec / denom; erased by spec denominator
    }

    // https://www.youtube.com/watch?v=wbBtAFpOxg8
    float MNDF_Schlick_GGX(float vec, float k){
        return 1.0 / (vec * (1.0-k) + k);
    }

//-----------------------------------------------------------------

void main() {
    vec2 Uv = scale*vUv;
    vec2 furUv = furScale*vUv;
    vec3 albedo = texture(diffuseMap, Uv).rgb;
    float furNoise = (valueNoise(furUv)-0.2)*1.25;
    //float furNoise = valueNoise(furUv);

    if(vLayer > 0.0)
    {
        if(furNoise < vLayer)
            discard;
    }
    float selfShadow = mix(0.1, 1.0, pow(vLayer, 0.5));

       // ---- Step 2: Geometric terms ----------------------------------------
    // Re-normalise vWorldNormal: linear interpolation across a triangle
    // can slightly denormalise a unit vector.
    vec3  N       = normalize(vWorldNormal);

    // L: unit vector FROM the surface point TOWARD the light source
    vec3  toLight = uLightPos - vWorldPos;
    float dist    = length(toLight);
    vec3  L       = toLight / dist;

    // ---- Step 3: Incident radiance (L_i) --------------------------------
    // A point light emits uniformly in all directions.
    // By the inverse-square law, irradiance falls off as 1/d²:
    //   L_i = (lightColor × intensity) / d²

    vec3 L_i = (uLightColor * uLightIntensity) / (dist * dist);

    // ---- Step 4: Lambertian BRDF (f_r) ----------------------------------
    // The BRDF describes how a surface scatters incoming light.
    // A Lambertian (perfectly matte) surface scatters light equally in
    // ALL outgoing directions, so f_r is a constant:
    //
    //   f_r = albedo / PI
    //
    // The 1/PI normalisation factor ensures energy conservation:
    // integrating f_r · cos(θ) over the full hemisphere gives ≤ 1,
    // so the surface never reflects more energy than it receives.

    vec3 f_r = albedo / PI;

    // ---- Step 5: Rendering equation (L_o) -------------------------------
    // Outgoing radiance toward the viewer:
    //   L_o = f_r × L_i × cos(θ_i)
    //       = f_r × L_i × max(0, N·L)
    //
    // max(0, …) clamps back-facing contributions (θ_i > 90°) to zero.

    float NdotL = max(0.0, dot(N, L));
    vec3  L_o   = uLightEnabled ? f_r * L_i * NdotL : vec3(0.0);

    // ---- Step 6: Ambient term -------------------------------------------
    // A simple constant ambient term approximates light bouncing from
    // the rest of the scene (indirect illumination).

    vec3 L_ambient = uAmbient * albedo;

    // ---- Step 7: Final output -------------------------------------------
    fragColor = vec4(selfShadow*(L_o + L_ambient), 1.0);
}