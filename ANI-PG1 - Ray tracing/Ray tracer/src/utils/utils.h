#pragma once

/*
    utils.h
    Common rendering utility helpers: progress reporting, gamma/exposure
    processing, PPM image output, time formatting, and triangle-ray
    intersection using the Möller–Trumbore algorithm.
*/

#include <cmath>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <string>
#include <iostream>

#include "constants.h"
#include "../math/vec3.h"
#include "../core/ray.h"
#include "../core/triangle.h"
#include "cfgLoader.h"

/**
 * @brief Print a progress message at key scanline milestones.
 *
 * @param y Current scanline index.
 */
inline void progress(uint32_t y){
    if (y == cfg.HEIGHT / 4)
        std::cout << "\rRendering: 25%"<< std::flush;
    else if (y == cfg.HEIGHT / 2)
        std::cout << "\rRendering: 50%"<< std::flush;
    else if (y == 3 * cfg.HEIGHT / 4)
        std::cout << "\rRendering: 75%"<< std::flush;
    else if (y == 90 * cfg.HEIGHT / 100)
        std::cout << "\rRendering: 90%"<< std::flush;
    else if (y == cfg.HEIGHT - 1)
        std::cout << "\rRendering: 100%"<< std::endl;
}

/**
 * @brief Clamp a floating-point value to the inclusive range [low, high].
 *
 * @param val Value to clamp.
 * @param low Lower bound.
 * @param high Upper bound.
 * @return Clamped float value.
 */
inline float clamp(float val, float low = 0.0f, float high = 1.0f){
    return std::max(std::min(val, high), low);
}

/**
 * @brief Clamp an unsigned integer to the inclusive range [low, high].
 *
 * @param val Value to clamp.
 * @param low Lower bound.
 * @param high Upper bound.
 * @return Clamped uint32_t value.
 */
inline uint32_t clamp(uint32_t val, uint32_t low = 0, uint32_t high = 1){
    return std::max(std::min(val, high), low);
}

/**
 * @brief Apply a gamma-like clamp to a single color channel.
 *
 * @param val Input channel value.
 * @return Tone-mapped channel value.
 */
inline float gammaClamp(float val){
    float power = 0.45454545f; // 1/2.2
    return clamp(std::pow((val/(val+1.0f)), power));
}

/**
 * @brief Apply the ACES filmic tone mapping curve to a color.
 *
 * @param x Input HDR color.
 * @return Tonemapped color in [0,1] range.
 */
inline vec3 ACESFilm(const vec3& x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    
    vec3 result;
    result.x = (x.x * (a * x.x + b)) / (x.x * (c * x.x + d) + e);
    result.y = (x.y * (a * x.y + b)) / (x.y * (c * x.y + d) + e);
    result.z = (x.z * (a * x.z + b)) / (x.z * (c * x.z + d) + e);

    return result;
}

/**
 * @brief Apply exposure, tone mapping, and gamma correction to a pixel.
 *
 * @param color Linear HDR color value.
 * @return Post-processed display-ready color.
 */
inline vec3 postProcessPixel(vec3 color) {
    
    color = color * cfg.EXPOSURE;
    color = ACESFilm(color);
    color.x = gammaClamp(color.x);
    color.y = gammaClamp(color.y);  
    color.z = gammaClamp(color.z);

    return color;
}

/**
 * @brief Return a formatted timestamp string for output filenames.
 *
 * @return Timestamp string with format dd-mm-YYYY_HH-MM.
 */
inline std::string getSimpleTime() {
    std::time_t t = std::time(nullptr);
    std::tm time_info; 

    #ifdef _WIN32
        localtime_s(&time_info, &t); 
    #else
        localtime_r(&t, &time_info); 
    #endif

    std::stringstream ss;
    ss << std::put_time(&time_info, "%d-%m-%Y_%H-%M");
    return ss.str();
}

/**
 * @brief Write the rendered image buffer to a PPM file.
 *
 * @param image Pointer to the RGB image buffer.
 */
inline void writeToPPM(float* image){
   std::string filename = "output/render_" + getSimpleTime() + ".ppm";
    std::cout << "Saving to " << filename << "...\n";

    std::ofstream output(filename);
    if (!output.is_open()) {
        std::cerr << "Failed to create file '" << filename << "'!\n";
        return; 
    }

    output << "P3\n" << cfg.WIDTH << " " << cfg.HEIGHT << "\n255\n";
    for (uint32_t i = 0; i < cfg.WIDTH * cfg.HEIGHT; ++i) {
        output << (int)(image[3*i]*255) << " " 
               << (int)(image[3*i+1]*255) << " " 
               << (int)(image[3*i+2]*255) << "\n";
    }
    output.close();
}

/**
 * @brief Perform the Möller–Trumbore triangle intersection test.
 *
 * @param ray Ray to test.
 * @param tri Triangle primitive.
 * @param cullback If true, backface culling is enabled.
 * @param b1 Barycentric coordinate output for the hit.
 * @param b2 Barycentric coordinate output for the hit.
 * @return Parametric distance t along the ray, or INF if no hit.
 */
inline float intersect(const sRay& ray, const sTriangle& tri, bool cullback, float& b1, float& b2) {
    vec3 a = tri._vertices[0], b = tri._vertices[1], c = tri._vertices[2];
    vec3 e1 = b - a;
    vec3 e2 = c - a;
    
    vec3 pvec = cross(ray._dir, e2); // s x e2
    float det = dot(e1, pvec); 
    // det > 0 ray hit the tri in opposite dir from tris normal
    // det < 0 ray hit the tri from behind
    // det == 0 ray goes parallel to triangle face
    
    if (cullback) {
        if (det < fEpsilon) 
            return INF;
    } else {
        if (std::abs(det) < fEpsilon) 
            return INF;
    }
    
    float invDet = 1.0f / det;
    vec3 tvec = ray._origin - a; // q = o - a
    
    b1 = dot(tvec, pvec) * invDet;
    if (b1 < 0.0f || b1 > 1.0f) 
        return INF;
    
    vec3 qvec = cross(tvec, e1); // r = q x e1
    b2 = dot(ray._dir, qvec) * invDet; // v = (r . s) * invDet
    if (b2 < 0.0f || b1 + b2 > 1.0f) 
        return INF;
    
    // compute t to find out where the intersection point is on the line
    return dot(e2, qvec) * invDet;
}