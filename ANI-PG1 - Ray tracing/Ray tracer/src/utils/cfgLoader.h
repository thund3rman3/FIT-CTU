#pragma once

/*
    cfgLoader.h
    Configuration loader for the renderer. Parses JSON settings and initializes
    camera parameters, image dimensions, and rendering options.
*/

#include <string>
#include <fstream>
#include <iostream>

#include "../../external/json.hpp"
#include "../scene/camera.h"
#include "../math/vec3.h"

using json = nlohmann::json;

struct ConfigData {
    uint32_t WIDTH = 600;
    uint32_t HEIGHT = 600;
    std::string SCENE_PATH = "scenes/sphere/Sphere.obj";
    std::string MAT_PATH = "scenes/sphere/";
    uint32_t REC_DEPTH_MAX = 8;
    uint32_t LIGHT_SAMPLES = 32;
    bool GGX = false;
    float EXPOSURE = 1.5f;
    float DIST = 6.0f;
};

inline ConfigData cfg;

inline bool loadConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filepath << ". Using default values.\n";
        return false;
    }

    try {
        json j;
        file >> j;

        if (j.contains("GGX")) 
            cfg.GGX = j["GGX"];
        if (j.contains("EXPOSURE")) 
            cfg.EXPOSURE = j["EXPOSURE"];
        if (j.contains("WIDTH")) 
            cfg.WIDTH = j["WIDTH"];
        if (j.contains("HEIGHT")) 
            cfg.HEIGHT = j["HEIGHT"];
        if (j.contains("CAMERA")) {
            const auto& camJson = j["CAMERA"];
            if (camJson.contains("pos") && 
                camJson.contains("up") && 
                camJson.contains("dir") && 
                camJson.contains("fov")) {
                vec3 pos = vec3(camJson["pos"][0], camJson["pos"][1], camJson["pos"][2]);
                vec3 up = vec3(camJson["up"][0], camJson["up"][1], camJson["up"][2]);
                vec3 dir = vec3(camJson["dir"][0], camJson["dir"][1], camJson["dir"][2]);
                float fov = camJson["fov"];
                camera = sCamera(pos, up, dir, fov, cfg.WIDTH, cfg.HEIGHT);
            }
        }

        if (j.contains("DMAX")) 
            cfg.REC_DEPTH_MAX = j["DMAX"];
        if (j.contains("LIGHT_SAMPLES")) 
            cfg.LIGHT_SAMPLES = j["LIGHT_SAMPLES"];
        if (j.contains("DIST")) 
            cfg.DIST = j["DIST"];
        if (j.contains("scenePath")) 
            cfg.SCENE_PATH = j["scenePath"];
        if (j.contains("mtlPath")) 
            cfg.MAT_PATH = j["mtlPath"];

        std::cout << "Config loaded from " << filepath << "\n";
        return true;
    } catch (const json::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << '\n';
        return false;
    }
}