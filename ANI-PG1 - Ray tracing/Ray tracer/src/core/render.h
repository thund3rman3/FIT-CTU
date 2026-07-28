#pragma once

/*
    render.h
    Renderer interface and ray tracing entrypoint declaration. Contains
    the main render method and internal shading utility prototypes.
*/

#include "../utils/objLoader.h"
#include "../accel/kdTreeTriang.h"
#include "hit.h"
#include "../utils/constants.h"
#include "../scene/camera.h"

// Phong shading
            // Il = light intensity/color, light.col
            // float I_d = I_l * r_d * dot(d_l, n);
            // float I_s = I_l * r_s * std::pow(dot(v, r_l), shininess);
            // I = I_a + I_d + I_s + I_r + I_t;

            // IR = barva od odražené složky světla
            // IT = barva od refrakční složky světla
            
            // float I_r = I_R * r_s; // I_R reflected light intensity
            // float I_t = I_T * T; // I_T refracted light intensity
            //I = I_r + I_t + ...; // missing

class cRenderer{
public:
    /**
     * @brief Construct a new renderer from loaded scene data.
     *
     * @param loader Reference to the object loader containing scene geometry.
     */
    cRenderer(const sObjLoader& loader)
        : _loader(&loader), _kdTree(loader._sceneTriangles), _image(new float[cfg.WIDTH * cfg.HEIGHT * 3]){
    }

    /**
     * @brief Destroy the renderer and release the image buffer.
     */
    ~cRenderer(){
        delete[] _image;
    }

    /**
     * @brief Execute the rendering pipeline.
     *
     * @return Pointer to the rendered RGB image buffer.
     */
    float* render();

private:
    const sObjLoader *_loader;
    cKdTreeTris _kdTree;
    float *_image;

    /**
     * @brief Trace a ray through the scene and evaluate shading recursively.
     *
     * @param ray Ray to trace.
     * @param depth Current recursion depth.
     * @param cullback Enable backface culling for triangle intersections.
     * @return Shaded color for the ray.
     */
    vec3 traceRay(const sRay& ray, uint32_t depth, bool cullback);

    /**
     * @brief Evaluate BRDF contribution for an area light hit.
     *
     * @param p Hit position.
     * @param n Surface normal.
     * @param v View direction.
     * @param uv Surface UV coordinates.
     * @param hitMat Material index of hit surface.
     * @param texScale Texture scale factor.
     * @param cullback Enable backface culling.
     * @return Shaded color contribution from the area light.
     */
    vec3 areaLightEvalBRDF(const vec3& p, const vec3& n, const vec3& v, const vec3& uv, const int hitMat , float texScale, bool cullback);

    /**
     * @brief Cast shadow rays toward an emitting triangle and accumulate shading.
     *
     * @param litTri Light triangle used for visibility tests.
     * @param dirPL Direction from point to light.
     * @param p Shading point.
     * @param n Surface normal.
     * @param v View direction.
     * @param uv Surface UV coordinates.
     * @param hitMat Material index of hit surface.
     * @param texScale Texture scale factor.
     * @param cullback Enable backface culling.
     * @return Shadowed color contribution.
     */
    vec3 castShadowRaysEvalBRDF(const sTriangleLight& litTri, const vec3& dirPL,const vec3& p, const vec3& n, const vec3& v, const vec3& uv, const int hitMat, float texScale, bool cullback);

    /**
     * @brief Evaluate Phong BRDF for a surface point.
     *
     * @param n Surface normal.
     * @param v View direction.
     * @param d_l Light direction.
     * @param r_d Diffuse reflection color.
     * @param hitMat Material index.
     * @return Resulting Phong shaded color.
     */
    vec3 Phong(const vec3& n, const vec3& v, const vec3& d_l, const vec3& r_d, const int hitMat);

    /**
     * @brief Evaluate GGX microfacet BRDF for a surface.
     *
     * @param n Surface normal.
     * @param V View direction.
     * @param L Light direction.
     * @param r_d Diffuse color.
     * @param hitMat Material index.
     * @return GGX shaded color.
     */
    vec3 GGX(const vec3& n, const vec3& V, const vec3& L, const vec3& r_d, const int hitMat);

    /**
     * @brief Compute inside/outside interaction and cast a reflection ray.
     *
     * @param n Surface normal, updated for interior hits.
     * @param rayDir Incoming ray direction.
     * @param p Hit position.
     * @param dotNV Cosine between normal and view direction.
     * @param IOR Index of refraction used for Fresnel.
     * @param depth Recursive depth.
     * @param cullback Enable backface culling.
     * @return Reflected color contribution.
     */
    vec3 setInsideAndCastReflectionRay(vec3& n, const vec3& rayDir, const vec3& p, float& dotNV, float& IOR, uint32_t depth, uint32_t cullback);

    /**
     * @brief Cast a refraction ray and compute transmitted color.
     *
     * @param rayDir Incoming ray direction.
     * @param n Surface normal.
     * @param p Hit position.
     * @param cr Refraction direction.
     * @param transmit Transmission color.
     * @param approx Fresnel precomputed terms.
     * @param dotNV Cosine between normal and view direction.
     * @param IOR Index of refraction.
     * @param depth Recursive depth.
     * @param cullback Enable backface culling.
     * @return Refracted color contribution.
     */
    vec3 castRefractionRay(const vec3& rayDir, const vec3& n, const vec3& p, const vec3& cr, const vec3& transmit, const std::pair<float, float>& approx, const float dotNV, const float IOR, const uint32_t depth, const uint32_t cullback);
    
    /**
     * @brief Cast secondary reflection or refraction rays for indirect lighting.
     *
     * @param c Incoming color.
     * @param n Surface normal.
     * @param rayDir Ray direction.
     * @param p Hit position.
     * @param hitMat Material index.
     * @param depth Recursive depth.
     * @param cullback Enable backface culling.
     * @return Indirect lighting contribution.
     */
    vec3 castSecondaryRays(const vec3& c, vec3& n, const vec3& rayDir, const vec3& p, const int hitMat, const uint32_t depth, const uint32_t cullback);

    /**
     * @brief Compute Fresnel reflection terms used for shading.
     *
     * @param dotNV Cosine of angle between normal and view direction.
     * @param IOR Index of refraction.
     * @return Pair of Fresnel coefficients for reflection.
     */
    std::pair<float, float> calcFresnelParts(const float dotNV, const float IOR);

    /**
     * @brief Find the closest intersection and compute hit surface details.
     *
     * @param p Output hit position.
     * @param n Output interpolated surface normal.
     * @param ray Input ray.
     * @param uvHit Output UV coordinates at hit.
     * @param hitMat Output material index.
     * @param cullback Enable backface culling.
     * @param texScale Output texture scaling factor.
     */
    void findIntersection(vec3& p, vec3& n, const sRay& ray, vec3& uvHit, int& hitMat, bool cullback, float& texScale);

    /**
     * @brief Interpolate UV coordinates across a triangle.
     *
     * @param tri Triangle primitive.
     * @param B0 First barycentric weight.
     * @param B1 Second barycentric weight.
     * @param B2 Third barycentric weight.
     * @return Interpolated UV coordinate.
     */
    vec3 interpolateUV(const sTriangle& tri, float B0, float B1, float B2);
    
    /**
     * @brief Transform the shading normal using tangent-space normal mapping.
     *
     * @param n Base surface normal.
     * @param t Tangent vector.
     * @param uvHit Surface UV coordinate at hit.
     * @param hitMat Material index.
     * @return Transformed normal vector.
     */
    vec3 transformNormal(const vec3& n, const vec3& t, const vec3& uvHit, const int hitMat);

};