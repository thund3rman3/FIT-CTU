#pragma once

/*
    texture.h
    Texture loading and sampling utilities, including bilinear and trilinear
    filtering, mipmap generation, and UV wrapping for surface shading.
*/

#include <vector>
#include <string>
#include <iostream>
#include <cmath>

#include "../../external/stb_image.h"
#include "utils/utils.h"
#include "../utils/cfgLoader.h"
#include "camera.h"
#include "../utils/logger.h"

/*
    Adressing of texture data:
    - clamping - clamp(u, 0, 1) 
    - reapeat - u = 1 - (u - std::floor(u)) | u<0, 
                 u = u - std::floor(u) | u>=0 
    - mirroring - u = u - std::floor(u) | if (std::floor(u) % 2 == 0),
                  u = 1 - (u - std::floor(u)) | if (std::floor(u) % 2 != 0)
*/

/**
 * @brief Wrap UV coordinates into the [0,1] range using repeating behavior.
 *
 * @param coord Input texture coordinate.
 * @return Wrapped coordinate in [0,1].
 */
inline float repeatUV(float coord){
    return coord < 0.0f ? 1.0f - (coord - std::floor(coord)) : coord - std::floor(coord);
}

/**
 * @brief Linearly interpolate between two 3D vectors.
 *
 * @param a First value.
 * @param b Second value.
 * @param t Interpolation factor.
 * @return Interpolated vector.
 */
inline vec3 lerp(const vec3& a, const vec3& b, float t) {
    return a + (b - a) * t;
}


class cTexture{
public:
    /**
     * @brief Load a texture from file and optionally generate mipmaps.
     *
     * @param filename Texture image filename.
     * @param genMipMaps Enable mipmap generation.
     */
    cTexture(const std::string& filename, bool genMipMaps = true);
    
    /**
     * @brief Retrieve the raw texel color at integer coordinates.
     *
     * @param x Texel x coordinate.
     * @param y Texel y coordinate.
     * @param level Mipmap level.
     * @return Texel color as vec3.
     */
    vec3 getTexelColor(int x, int y, int level) const {
        int idx = (y * _texMipMaps[level]._width + x) * 3;
        return vec3(
            _texMipMaps[level]._texture[idx],
            _texMipMaps[level]._texture[idx + 1],
            _texMipMaps[level]._texture[idx + 2]
        );
    }

    /**
     * @brief Sample the texture with bilinear filtering at a given mipmap level.
     *
     * @param u Horizontal UV coordinate.
     * @param v Vertical UV coordinate.
     * @param level Mipmap level.
     * @return Filtered color.
     */
    vec3 sampleBilinearUV(float u, float v, uint32_t level) const;

    /**
     * @brief Sample the texture using trilinear filtering and mipmap selection.
     *
     * @param u Horizontal UV coordinate.
     * @param v Vertical UV coordinate.
     * @param distance Distance from camera to shading point.
     * @param texScale Texture scale factor.
     * @return Filtered color after mipmap blending.
     */
    vec3 sampleTrilinearUV(float u, float v, float distance, float texScale) const;

    /**
     * @brief Generate mipmap levels for the loaded texture.
     *
     * Uses 2x2 averaging to build each lower-resolution level.
     */
    void generateMipMaps();

private:
    struct sMipMapLevel
    {
        uint32_t _width, _height;
        std::vector<float> _texture;

        sMipMapLevel(uint32_t w, uint32_t h) 
            : _width(w), _height(h), _texture(w * h * 3) {}
    };

    std::vector<sMipMapLevel> _texMipMaps;
    bool _usesMipMaps;
};