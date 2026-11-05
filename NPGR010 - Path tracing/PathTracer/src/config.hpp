#pragma once

#include <vector>
#include <cmath>
#include <time.h>
#include <cstdlib>
#include "math.hpp"
#include "ray.hpp"
#include "geometry.hpp"
#include "camera.hpp"
#include "framebuffer.hpp"
#include "scene.hpp"
#include "pathtracer.hpp"

#include <omp.h>
#include <string>
#include <set>
#include <sstream>

// Renderer configuration, holds algorithm, scene, and all other settings
struct Config {
    // Parameters marked with [cmd] can be change from command line
    const Scene *mScene; // [cmd] When NULL, renderer will not run
    Framebuffer *mFramebuffer;
    std::string mOutputName; // [cmd]
    Vec2i mResolution;
    int mIterations; // [cmd]
    int mNumThreads;
    int mBaseSeed;
    uint mMaxPathLength;
    uint mMinPathLength;

    Config()
        : mScene(nullptr), mFramebuffer(nullptr), mResolution(512, 512), mIterations(1),
          mNumThreads(std::max(1, omp_get_num_procs())), mBaseSeed(1234), mMaxPathLength(10), mMinPathLength(0) {
    }

    ~Config() {
        delete mScene;
    }
};

// Utility function, essentially a renderer factory
inline std::unique_ptr<AbstractRenderer> CreateRenderer(const Config &aConfig, const int aSeed) {
    const Scene &scene = *aConfig.mScene;
    return std::make_unique<PathTracer>(scene, aSeed);
}

// Scene configurations
inline uint g_SceneConfigs[] = {
    Scene::kLightPoint | Scene::kWalls | Scene::kSpheres | Scene::kWallsDiffuse | Scene::kSpheresDiffuse,
    Scene::kLightPoint | Scene::kWalls | Scene::kSpheres | Scene::kWallsDiffuse | Scene::kSpheresDiffuse |
    Scene::kWallsGlossy | Scene::kSpheresGlossy,
    Scene::kLightCeiling | Scene::kWalls | Scene::kSpheres | Scene::kWallsDiffuse | Scene::kSpheresDiffuse,
    Scene::kLightCeiling | Scene::kWalls | Scene::kSpheres | Scene::kWallsDiffuse | Scene::kSpheresDiffuse |
    Scene::kWallsGlossy | Scene::kSpheresGlossy,
    Scene::kLightBox | Scene::kWalls | Scene::kSpheres | Scene::kWallsDiffuse | Scene::kSpheresDiffuse,
    Scene::kLightBox | Scene::kWalls | Scene::kSpheres | Scene::kWallsDiffuse | Scene::kSpheresDiffuse |
    Scene::kWallsGlossy | Scene::kSpheresGlossy,
    Scene::kLightEnv | Scene::kWalls | Scene::kSpheres | Scene::kWallsDiffuse | Scene::kSpheresDiffuse,
    Scene::kLightEnv | Scene::kWalls | Scene::kSpheres | Scene::kWallsDiffuse | Scene::kSpheresDiffuse |
    Scene::kWallsGlossy | Scene::kSpheresGlossy
};

inline std::string DefaultFilename(const int sceneID, const Scene &aScene) {
    std::string filename;

    filename += std::to_string(sceneID);
    filename += "_";

    // We use scene acronym
    filename += aScene.mSceneAcronym;

    // And it will be written as hdr
    filename += ".hdr";

    return filename;
}

// Utility function, gives length of array
template<typename T, size_t N>
inline int SizeOfArray(const T (&)[N]) {
    return static_cast<int>(N);
}

inline void PrintRngWarning() {
#if defined(LEGACY_RNG)
    printf("The code was not compiled for C++11.\n");
    printf("It will be using Tiny Encryption Algorithm-based" "random number generator.\n");
    printf("This is worse than the Mersenne Twister from C++11.\n"); printf("Consider setting up for C++11.\n"); printf(
        "Visual Studio 2010, and g++ 4.6.3 and later work.\n\n");
#endif
}

inline void PrintHelp(const char *argv[]) {
    printf("\n");
    printf("Usage: %s -s <scene_id> [ -i <iterations> | -o <output_name> ]\n\n", argv[0]);
    printf("    -s  Selects the scene:\n");

    for (int i = 0; i < SizeOfArray(g_SceneConfigs); i++)
        printf("          %d    %s\n", i, Scene::GetSceneName(g_SceneConfigs[i]).c_str());

    printf("    -i  Number of iterations to run the algorithm (default 1)\n");
    printf("    -o  User specified output name, with extension .hdr, .pfm, or .bmp (default .hdr)\n");
}

// Parses command line, setting up config
inline void ParseCommandline(int argc, const char *argv[], Config &oConfig) {
    int sceneID = -1; // defaults to no scene

    // Load arguments
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);

        // print help string (at any position)
        if (arg == "-h" || arg == "--help" || arg == "/?") {
            PrintHelp(argv);
            return;
        }

        if (arg[0] != '-') // all our commands start with -
        {
            continue;
        }
        else if (arg == "-s") // scene to load
        {
            if (++i == argc) {
                printf("Missing <sceneID> argument, please see help (-h)\n");
                return;
            }

            std::istringstream iss(argv[i]);
            iss >> sceneID;

            if (iss.fail() || sceneID < 0 || sceneID >= SizeOfArray(g_SceneConfigs)) {
                printf("Invalid <sceneID> argument, please see help (-h)\n");
                return;
            }
        }
        else if (arg == "-i") // number of iterations to run
        {
            if (++i == argc) {
                printf("Missing <iterations> argument, please see help (-h)\n");
                return;
            }

            std::istringstream iss(argv[i]);
            iss >> oConfig.mIterations;

            if (iss.fail() || oConfig.mIterations < 1) {
                printf("Invalid <iterations> argument, please see help (-h)\n");
                return;
            }
        }
        else if (arg == "-o") // output name
        {
            if (++i == argc) {
                printf("Missing <output_name> argument, please see help (-h)\n");
                return;
            }

            oConfig.mOutputName = argv[i];

            if (oConfig.mOutputName.empty()) {
                printf("Invalid <output_name> argument, please see help (-h)\n");
                return;
            }
        }
    }

    if (sceneID < 0) {
        PrintHelp(argv);
        return;
    }

    // Load scene
    Scene *scene = new Scene;
    scene->LoadCornellBox(oConfig.mResolution, g_SceneConfigs[sceneID]);

    oConfig.mScene = scene;

    // If no output name is chosen, create a default one
    if (oConfig.mOutputName.empty()) {
        oConfig.mOutputName = DefaultFilename(sceneID, *oConfig.mScene);
    }

    // Check if output name has valid extension (.bmp or .hdr) and if not add .bmp
    std::string extension;

    if (oConfig.mOutputName.length() > 4) // must be at least 1 character before .bmp
        extension = oConfig.mOutputName.substr(oConfig.mOutputName.length() - 4, 4);

    if (extension != ".bmp" && extension != ".hdr" && extension != ".pfm" && extension != ".ppm")
        oConfig.mOutputName += ".hdr";
}
