#include "render.h"

float* cRenderer::render(){
    if(cfg.GGX){
        std::cout << "Rendering with GGX..." << std::endl;
        for (uint32_t y = 0; y < cfg.HEIGHT; ++y) {
            progress(y);
            for (uint32_t x = 0; x < cfg.WIDTH; ++x) {
                vec3 pvec = (camera._startPvec + camera._stepRight * (float)x + camera._stepUp * (float)y).normalize(); // Calculate the point on the image plane corresponding to pixel (x, y)
                sRay ray(camera._pos, pvec);

                ++logger._cntPrim;
                vec3 I = traceRay(ray, 0, true);
                I = postProcessPixel(I);
                uint32_t idx = 3 * (y * cfg.WIDTH + x);
                _image[idx] = I.x;
                _image[idx+1] = I.y;
                _image[idx+2] = I.z;
            }
        }        
    }
    else{ // Phong
        std::cout << "Rendering with Phong shading..." << std::endl;
        for (uint32_t y = 0; y < cfg.HEIGHT; ++y) {
            progress(y);
            for (uint32_t x = 0; x < cfg.WIDTH; ++x) {
                vec3 pvec = (camera._startPvec + camera._stepRight * (float)x + camera._stepUp * (float)y).normalize(); // Calculate the point on the image plane corresponding to pixel (x, y)
                sRay ray(camera._pos, pvec);

                ++logger._cntPrim;
                vec3 I = traceRay(ray, 0, true);

                uint32_t idx = 3 * (y * cfg.WIDTH + x);
                _image[idx] = clamp(I.x);
                _image[idx+1] = clamp(I.y);
                _image[idx+2] = clamp(I.z);
            }
        }    
    }
    return _image;
}

vec3 cRenderer::traceRay(const sRay& ray, uint32_t depth, bool cullback){
    int hitMat = -1;
    vec3 p(INF);
    vec3 n; // Normal at hit point to interpolate lighting
    vec3 uv; // UV coordinates at hit point p for texture mapping
    float texScale;
    findIntersection(p, n, ray, uv, hitMat, cullback, texScale);

    if(hitMat == -1)
        return vec3();
    
    vec3 c = _loader->_sceneMaterials[hitMat]._emission;
    
    if (c.x > 0.0f || c.y > 0.0f || c.z > 0.0f)
        return c; // color of light
    
    vec3 v = (-ray._dir).normalize();  // direction from hit point P to origin

    if(cullback)
        c += areaLightEvalBRDF(p, n, v, uv, hitMat, texScale,cullback);

    //return c;
    return castSecondaryRays(c, n, ray._dir, p, hitMat, depth, cullback);
}

vec3 cRenderer::areaLightEvalBRDF(const vec3& p, const vec3& n, const vec3& v, const vec3& uv, const int hitMat , float texScale, bool cullback){
        
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<float> rnd(0.0f, 1.0f);
    vec3 res;
    for (size_t i = 0; i < _loader->_lightTriangles.size(); ++i)
    {       
        const sTriangleLight& litTri = _loader->_lightTriangles[i];
        vec3 a = litTri._vertices[0];
        vec3 b = litTri._vertices[1];
        vec3 c = litTri._vertices[2];
        vec3 tmp;

        for (size_t j = 0; j < cfg.LIGHT_SAMPLES; j++)
        {
            float r1 = rnd(gen);
            float r2 = rnd(gen);
            float b1 = r1 + r2 > 1.0f ? 1.0f - r1 : r1;
            float b2 = r1 + r2 > 1.0f ? 1.0f - r2 : r2;
            vec3 p_l = a + (b - a) * b1 + (c - a) * b2;
            vec3 dirPL = p_l - p; // direction p - light

            tmp += castShadowRaysEvalBRDF(litTri, dirPL, p, n, v, uv, hitMat, texScale, cullback); 
        }
        res += tmp * litTri._triArea / static_cast<float>(cfg.LIGHT_SAMPLES);
    }
    return res;
}

vec3 cRenderer::castShadowRaysEvalBRDF(const sTriangleLight& litTri, const vec3& dirPL,const vec3& p, const vec3& n, const vec3& v, const vec3& uv, const int hitMat, float texScale, bool cullback){

    float dstPL = dirPL.length(); // dist p - light
    vec3 dirNormalizedPL = dirPL / dstPL; // dir p - light normalized
    sRay shadowRay(p + n * gEpsilon, dirNormalizedPL);
    ++logger._cntShad;
    vec3 tmp;

    auto start = std::chrono::steady_clock::now();

    sHitInfo hitInfo = _kdTree.traverse(shadowRay, cullback, true, dstPL - lEpsilon);

    bool inShadow = hitInfo.isHit();

    logger.logRayIntersection(start, false);

    if(!inShadow) {
        vec3 r_d = _loader->_sceneMaterials[hitMat]._diffuse;
        cTexture* albedoTex = _loader->_sceneMaterials[hitMat]._albedoTex.get();
        if(albedoTex != nullptr){
            float dstCP = (p - camera._pos).length();
            r_d = albedoTex->sampleTrilinearUV(uv.x, uv.y, dstCP, texScale);
        }

        vec3 color = cfg.GGX ? GGX(n, v, dirNormalizedPL, r_d, hitMat) : Phong(n, v, dirNormalizedPL, r_d, hitMat);
        float denom = dstPL *dstPL;
        float num = std::max(0.0f, dot(n, dirNormalizedPL)) * std::max(0.0f, dot(litTri._faceNormal, -dirNormalizedPL));
        tmp += color * litTri._emission * num / denom;
    }

    return tmp;
}

vec3 cRenderer::Phong(const vec3& n, const vec3& v, const vec3& d_l, const vec3& r_d, const int hitMat){
    vec3 r_s = _loader->_sceneMaterials[hitMat]._specular;
    float h = _loader->_sceneMaterials[hitMat]._shininess;

    vec3 r_l = (n * (2 * dot(d_l, n)) - d_l).normalize();  // direction of reflected light, dot must be -1,1 to move vectors -> badly turned noramls will be eliminated by w or others
    float spec = std::pow(std::max(0.0f, dot(v, r_l)), h); // dot > 0 to fit pow

    // I = I_l * (r_d*DLdotN + r_s*(VdotRL)^h) 
    return (r_d + r_s * spec );//  * std::max(0.0f, dot(n, d_l)), I_l outside
}

vec3 cRenderer::GGX(const vec3& n, const vec3& V, const vec3& L, const vec3& r_d, const int hitMat) {
    vec3 F0 = _loader->_sceneMaterials[hitMat]._specular;
    float h = _loader->_sceneMaterials[hitMat]._shininess;

    // Conversion of Phong shininess (0-1000) to PBR roughness (0-1)
    float roughness = std::sqrt(2.0f / (h + 2.0f));
    float alpha = roughness * roughness;

    float dotNV = std::max(dot(n, V), 0.0f);
    float dotNL = std::max(dot(n, L), 0.0f);
    
    if (dotNL <= 0.0f) {
        return vec3();
    }

    vec3 H = (V + L).normalize(); // Half-vector
    float dotNH = std::max(dot(n, H), 0.0f);
    float dotLH = std::max(dot(L, H), 0.0f);

    // D - Distribution Term (GGX)
    float alpha2 = alpha * alpha;
    float denom = (dotNH * dotNH * (alpha2 - 1.0f) + 1.0f);
    float D = alpha2 / (PI * denom * denom);

    // G - Geometry Term (Schlick-GGX)
    float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
    float G = 1.0f / (4.0f * (dotNL * (1.0f - k) + k)*(dotNV * (1.0f - k) + k));

    // F - Fresnel Term (Schlick)
    float dotLHi = 1.0f - dotLH;
    float dotLHi2 = dotLHi * dotLHi;
    float dotLHi5 = dotLHi2 * dotLHi2 * dotLHi;
    vec3 F = F0 + (vec3(1.0f) - F0) * dotLHi5;

    vec3 s = F;         
    vec3 d = vec3(1.0f) - s; 
    
    vec3 diffuse = d * (r_d / PI);
    vec3 specular = F * D * G;

    // (diffuse + specular) * lights[i].col * dotNL;
    return (diffuse + specular); // dotNL, Il outside
}

vec3 cRenderer::setInsideAndCastReflectionRay(vec3& n, const vec3& rayDir, const vec3& p, float& dotNV, float& IOR, uint32_t depth, uint32_t cullback){

    if(dotNV > 0.0f){ // 0-90 degrees, from mat to air
        n = -n;
    }
    else{ // 90-180, from air to mat
        IOR = 1.0f / IOR;
        dotNV = -dotNV;
    }

    vec3 reflectedDir = (rayDir - n * (2.0f * dot(rayDir, n))).normalize();
    sRay refl(p + n * gEpsilon, reflectedDir);
    ++logger._cntRefl;
    return traceRay(refl, depth + 1, cullback);
}

vec3 cRenderer::castRefractionRay(const vec3& rayDir, const vec3& n, const vec3& p, const vec3& cr, const vec3& transmit, const std::pair<float, float>& approx, const float dotNV, const float IOR, const uint32_t depth, const uint32_t cullback){
    float F = approx.first + (1.0f - approx.first) * approx.second;
    float k = 1.0f - IOR*IOR * (1.0f - dotNV*dotNV);
    vec3 ct;
    if(k >= 0.0f){
        vec3 refracted_dir = (rayDir * IOR + n * (dotNV * IOR - std::sqrt(k))).normalize();
        sRay refr(p - n * gEpsilon, refracted_dir);
        ++logger._cntRefr;
        ct = traceRay(refr, depth + 1, !cullback);
    }
    else{
        F = 1.0f; // TIR
        //return c + cr * transmit; //whitted RT
    } 
    return cr * F + ct * (1.0f - F) * transmit; // ct*transmit //whitted RT
}

vec3 cRenderer::castSecondaryRays(const vec3& c, vec3& n, const vec3& rayDir, const vec3& p, const int hitMat, const uint32_t depth, const uint32_t cullback){
    vec3 cr, ct;
    vec3 transmit = _loader->_sceneMaterials[hitMat]._transmittance;
    vec3 specular = _loader->_sceneMaterials[hitMat]._specular;
    bool transmits = (transmit.x > 0.0f || transmit.y > 0.0f || transmit.z > 0.0f);
    bool reflects = (specular.x > 0.0f || specular.y > 0.0f || specular.z > 0.0f);

    if(depth >= cfg.REC_DEPTH_MAX || (!transmits && !reflects))
        return c;

    float dotNV = dot(n, rayDir);
    float IOR = _loader->_sceneMaterials[hitMat]._IOR;
    cr = setInsideAndCastReflectionRay(n, rayDir, p, dotNV, IOR, depth, cullback);

    std::pair<float, float> approx = calcFresnelParts(dotNV, _loader->_sceneMaterials[hitMat]._IOR); // first=F0, sec=(1-dotNV)^5
    
    if(transmits){
        return castRefractionRay(rayDir, n, p, cr, transmit, approx, dotNV, IOR, depth, cullback);
    }
    // Use 'specular' (Ks from mat) as F0
    vec3 F = specular + (vec3(1.0f) - specular) * approx.second;
    return c * (vec3(1.0f) - F) + cr * F;

    //return c + cr * specular + ct * transmit; //whitted RT
}

std::pair<float, float> cRenderer::calcFresnelParts(const float dotNV, const float IOR){
    float F0 = (1.0f - IOR) / (1.0f + IOR);
    F0 = F0 * F0;
    float dotNVi = 1.0f - dotNV;
    float dotNVi2 = dotNVi * dotNVi;
    float dotNVi5 = dotNVi2 * dotNVi2 * dotNVi;
    return {F0, dotNVi5};
}

void cRenderer::findIntersection(vec3& p, vec3& n, const sRay& ray, vec3& uvHit, int& hitMat, bool cullback, float& texScale){
    auto start = std::chrono::steady_clock::now();
    sHitInfo hitInfo = _kdTree.traverse(ray, cullback);

    logger.logRayIntersection(start, true);

    if(!hitInfo.isHit())
        return;

    hitMat = hitInfo._hitTri->_matIdx;
    if(hitMat == -1)
        return;

    p = ray._origin + ray._dir * hitInfo._t; // Calculate the hit point P
    texScale = hitInfo._hitTri->_texScale;
    float B0 = 1.0f - hitInfo._b1 - hitInfo._b2;
    
    uvHit = interpolateUV(*hitInfo._hitTri, B0, hitInfo._b1, hitInfo._b2);

    n = hitInfo._hitTri->_normals[0] * B0 + hitInfo._hitTri->_normals[1] * hitInfo._b1 + hitInfo._hitTri->_normals[2] * hitInfo._b2; // Interpolate normal using barycentric coordinates
    n = n.normalize();
    vec3 t = hitInfo._hitTri->_tangents[0] * B0 + hitInfo._hitTri->_tangents[1] * hitInfo._b1 + hitInfo._hitTri->_tangents[2] * hitInfo._b2; // Interpolate tangent
    t = t.normalize();
    
    n = transformNormal(n, t, uvHit, hitMat);
}

vec3 cRenderer::interpolateUV(const sTriangle& tri, float B0, float B1, float B2){
    vec3 uv0 = tri._uvs[0];
    vec3 uv1 = tri._uvs[1];
    vec3 uv2 = tri._uvs[2];
    
    float u = B0 * uv0.x + B1 * uv1.x + B2 * uv2.x;
    float v = B0 * uv0.y + B1 * uv1.y + B2 * uv2.y;
    return vec3(u, v, 0.0f);
}

vec3 cRenderer::transformNormal(const vec3& n, const vec3& t, const vec3& uvHit, const int hitMat){
    
    cTexture* normalTex = _loader->_sceneMaterials[hitMat]._normalTex.get();
    if(!normalTex)
        return n; 

    vec3 newNormal = normalTex->sampleBilinearUV(uvHit.x, uvHit.y, 0);
        
    // Range [0, 1] -> [-1, 1]
    newNormal = newNormal * 2.0f - vec3(1.0f);
    newNormal = newNormal.normalize();

    // Gram-Schmidt reorthogonalization (tangent orthogonal to normal)
    vec3 tangent = (t - n * dot(n, t)).normalize();
    
    vec3 bitangent = cross(n, tangent).normalize();
    // TBN matrix transformation from tangent space to world space
    vec3 transformedNormal = (tangent * newNormal.x + bitangent * newNormal.y + n * newNormal.z);
    return transformedNormal.normalize();
}