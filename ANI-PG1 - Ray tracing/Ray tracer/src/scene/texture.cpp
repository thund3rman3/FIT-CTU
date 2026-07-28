#include "texture.h"

cTexture::cTexture(const std::string& filename, bool genMipMaps)
    : _usesMipMaps(genMipMaps) {
    int channels;
    int desired_channels = 3; // RGB
    int width, height;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, desired_channels);
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        width = height = 0;
        return;
    }
    sMipMapLevel baseLevel(width, height);
    std::copy(data, data + width * height * 3, baseLevel._texture.begin());
    
    if(cfg.GGX){
        for (int i = 0; i < width * height * 3; ++i) {
            baseLevel._texture[i] = std::pow(baseLevel._texture[i] / 255.0f, 2.2f);
        }
    }
    else{
        for (int i = 0; i < width * height * 3; ++i) {
            baseLevel._texture[i] = baseLevel._texture[i] / 255.0f;
        }
    }
    _texMipMaps.push_back(baseLevel);
    if (genMipMaps)
        generateMipMaps();
    
    stbi_image_free(data);
}

vec3 cTexture::sampleBilinearUV(float u, float v, uint32_t level) const {
    if (_texMipMaps[level]._width == 0 || _texMipMaps[level]._height == 0) {
        return vec3(1.0f); // Return white if texture failed to load
    }
    float x = repeatUV(u) * (_texMipMaps[level]._width - 1);
    float y = repeatUV(v) * (_texMipMaps[level]._height - 1);
    uint32_t xFloor = static_cast<int>(std::floor(x));
    uint32_t yFloor = static_cast<int>(std::floor(y));
    uint32_t xCeil = static_cast<int>(std::ceil(x));
    uint32_t yCeil = static_cast<int>(std::ceil(y));
    
    vec3 c_tl = getTexelColor(xFloor, yFloor, level);
    vec3 c_tr = getTexelColor(xCeil, yFloor, level);
    vec3 c_bl = getTexelColor(xFloor, yCeil, level);
    vec3 c_br = getTexelColor(xCeil, yCeil, level);

    float t_1 = x - std::floor(x);
    float t_2 = y - std::floor(y);

    return lerp(lerp(c_tl, c_tr, t_1), lerp(c_bl, c_br, t_1), t_2);
}

vec3 cTexture::sampleTrilinearUV(float u, float v, float distance, float texScale) const {
    if(!_usesMipMaps || _texMipMaps.size() == 1) {
        return sampleBilinearUV(u, v, 0);
    }

    float pixelSpreadAngle = camera._fov / cfg.HEIGHT;
    float footprintWorld = distance * pixelSpreadAngle; 
    
    float offset = 0.0f; // Bias
    float ratio = footprintWorld * texScale * _texMipMaps[0]._width; 
    float level = std::log2(ratio) - offset; 
    
    // debug Levels
    if(level > logger._maxMipLvl) logger._maxMipLvl = level;
    if(level < logger._minMipLvl) logger._minMipLvl = level;
    if(texScale > logger._maxTexSize) logger._maxTexSize = texScale;
    if(texScale < logger._minTexSize) logger._minTexSize = texScale;
    //

    uint32_t max_level = static_cast<int>(_texMipMaps.size()) - 1;

    if (level <= 0.0f) 
        return sampleBilinearUV(u, v, 0);
    if (level >= max_level) 
        return sampleBilinearUV(u, v, max_level);
    
    level = clamp(level, 0.0f, static_cast<float>(max_level));
    uint32_t levelLow = static_cast<int>(std::floor(level));
    float t_3 = level - levelLow;

    return lerp(sampleBilinearUV(u, v, levelLow), sampleBilinearUV(u, v, levelLow + 1), t_3);
}

void cTexture::generateMipMaps(){
    uint32_t prevW = _texMipMaps[0]._width;
    uint32_t prevH = _texMipMaps[0]._height;
    if((prevW & (prevW - 1)) != 0 || (prevH & (prevH - 1)) != 0) {
        std::cerr << "Texture dimensions should be powers of 2 for mipmapping." << std::endl;
        _usesMipMaps = false;
        return;
    }

    uint32_t prevLevel = 0;
    for (; prevW > 1 || prevH > 1; prevW /= 2, prevH /= 2)
    {
        sMipMapLevel mipMap(prevW/2, prevH/2);
        for (uint32_t x = 0; x < prevW; x+=2)
        { 
            for (uint32_t y = 0; y < prevH; y+=2)
            {
                vec3 c1 = getTexelColor(x, y, prevLevel);
                vec3 c2 = getTexelColor(x + 1, y, prevLevel);
                vec3 c3 = getTexelColor(x, y + 1, prevLevel);
                vec3 c4 = getTexelColor(x + 1, y + 1, prevLevel);
                uint32_t idx = (y/2 * mipMap._width + x/2) * 3;
                mipMap._texture[idx] = (c1.x + c2.x + c3.x + c4.x) * 0.25f;
                mipMap._texture[idx+1] = (c1.y + c2.y + c3.y + c4.y) * 0.25f;
                mipMap._texture[idx+2] = (c1.z + c2.z + c3.z + c4.z) * 0.25f;
            }
        }
        _texMipMaps.push_back(mipMap);
        ++prevLevel;
    }
    std::cout << "Generated " << _texMipMaps.size() << " mipmap levels." << std::endl;
}