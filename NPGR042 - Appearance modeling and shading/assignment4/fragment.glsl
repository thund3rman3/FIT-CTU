precision highp float;

uniform float bumpStrength;
uniform float scale;

// ---- Point light uniforms ----------------------------------------------
uniform vec3  uLightPos;       // light position in world space
uniform vec3  uLightColor;     // RGB color of the light
uniform float uLightIntensity; // light intensity (scales emitted power)
uniform float uAmbient;        // ambient light scale (approximates indirect lighting)
uniform bool  uLightEnabled;   // toggle direct lighting on/off

uniform sampler2D diffuseMap;  
uniform sampler2D normalMap;
uniform sampler2D roughMap;
uniform sampler2D specMap;

in vec2 vUv;
in vec3 vWorldPos;
in vec3 vWorldNormal;

in vec4 vWorldTangent; // surface tangent in world space (U direction)

out vec4 fragColor;

const float PI = 3.14159265358979;
 
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
    vec3 albedo = texture(diffuseMap, Uv).rgb;
    vec3 normal = texture(normalMap, Uv).rgb;
    float roughness = texture(roughMap, Uv).r;
    float spec = texture(specMap, Uv).r;

    // ---- Geometric terms ----------------------------------------
    vec3  N       = normalize(vWorldNormal); 
    vec3  T       = normalize(vWorldTangent.xyz);

    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N, T) * vWorldTangent.w);

    // ---- Bump mapping -------------------------------------------
    mat3 TBN = mat3(T, B, N);

    vec3 localNormal = (normal * 2.0 - 1.0);
    localNormal.xy *= bumpStrength;

    vec3 pN = normalize(TBN * normalize(localNormal));

    // L: unit vector FROM the surface point TOWARD the light source
    vec3  toLight = uLightPos - vWorldPos;
    float dist    = length(toLight);
    vec3  L       = toLight / dist;

    // ---- Incident radiance (L_i) --------------------------------
    // A point light emits uniformly in all directions.
    vec3 L_i = (uLightColor * uLightIntensity) / (dist * dist);

    // ---- Lambertian BRDF (f_diff) ----------------------------------
    // The BRDF describes how a surface scatters incoming light.
    // A Lambertian (perfectly matte) surface scatters light equally in
    // ALL outgoing directions, so f_r is a constant:
    vec3 f_diff = albedo / PI;


    float NdotL = max(0.0, dot(pN, L));
    vec3 V = normalize(cameraPosition - vWorldPos);
    vec3 H = normalize(L + V);
    float NdotV = max(0.0, dot(pN, V));
    float NdotH = max(0.0, dot(pN, H));
    float VdotH = max(0.0, dot(V, H));

    // ---- Specular term (f_spec) -------------------------------
    // Microfacet (normal) Distribution function D - GGX formula which depends on roughness and NdotH
    // Fresnel term (F) - Schlick’s Fresnel formula, which depends on the F0 term (Fresnel term at 0°, or basically the specular color) and VdotH
    // G - Smith-GGX formula - depends on roughness, NdotL, and NdotV

    float IOR = 1.5;
    vec3 F = schlick_approx(VdotH, IOR, spec);
    float D = NDF_GGX_D(roughness, NdotH);

    //float G = MNDF_Smith_GGX(NdotL, roughness)*MNDF_Smith_GGX(NdotV, roughness);
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0; // k direct
    float G = MNDF_Schlick_GGX(NdotL, k) * MNDF_Schlick_GGX(NdotV, k);

    // (D * G * F) < - (D * G * F) / max(4.0 * NdotL * NdotV, 0.001); erased by smith G GGX
    vec3 f_spec = (D * G * F) / 4.0; // erased by schlick G GGX 

    vec3 f_r = (1.0-F)*f_diff + f_spec;

    // ---- Rendering equation (L_o) -------------------------------
    // Outgoing radiance toward the viewer:
    // max(0, …) clamps back-facing contributions (θ_i > 90°) to zero.
    vec3  L_o   = uLightEnabled ? f_r * L_i * NdotL : vec3(0.0);

    // ---- Ambient term -------------------------------------------
    vec3 L_ambient = uAmbient * albedo;

    // ---- Step 7: Final output -------------------------------------------
    fragColor = vec4(L_o + L_ambient, 1.0);
    fragColor = pow(fragColor, vec4(1.0/2.2));
    //fragColor = vec4(vec3(heigh_map), 1.0);
}