#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG

#include <chrono>

#include "core/render.h"
#include "utils/logger.h"
#include "utils/objLoader.h"
#include "utils/utils.h" 
#include "utils/cfgLoader.h"
#include "utils/constants.h"

int main() {
    loadConfig(CONFIG_PATH);
    sObjLoader loader;
    cRenderer rayTracer(loader);

    auto start = std::chrono::steady_clock::now();
    float *image = rayTracer.render();
    logger.saveToLog(start);
    
    writeToPPM(image);
}