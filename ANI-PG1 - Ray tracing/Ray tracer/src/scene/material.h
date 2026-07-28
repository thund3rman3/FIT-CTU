#pragma once 

/*
    material.h
    Defines material properties for rendering, including diffuse,
    specular, transmittance, emission components, and texture maps.
*/

#include <memory>

#include "texture.h"
#include "../math/vec3.h"

struct sMaterial {
    std::shared_ptr<cTexture> _albedoTex; // Diffuse map
    std::shared_ptr<cTexture> _normalTex; // Normal map

    //vec3 ambient; // Ia - Ignored in this implementation
    vec3 _diffuse; // r_d
    vec3 _specular; // r_s
    vec3 _transmittance; // T
    vec3 _emission; // Ie

    float _shininess; // h 
    float _IOR; // 1/N_t
    //float dissolve; 
    //std::string name;

    sMaterial(const vec3& diffuse,
                const vec3& specular,
                const vec3& transmittance,
                const vec3& emission,
                float shininess,
                float ior) 
        : _albedoTex(nullptr), 
          _normalTex(nullptr),
          _diffuse(diffuse), 
          _specular(specular), 
          _transmittance(transmittance),
          _emission(emission), 
          _shininess(shininess), _IOR(ior) {}
};
