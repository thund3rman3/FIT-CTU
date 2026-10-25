#include "math.hpp"
#include "framebuffer.hpp"
#include "scene.hpp"
#include "pathtracer.hpp"
#include "config.hpp"

#include <omp.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <print>

void PrintProgressBar(int current, int total) {
    const double progress = static_cast<double>(current) / total;
    constexpr int barCount = 20;

    std::print("\rProgress:  {:6.2f}% [", 100.0 * progress);
    for (int bar = 1; bar <= barCount; bar++) {
        const double barProgress = static_cast<double>(bar) / barCount;
        if (barProgress <= progress) {
            std::print("|");
        } else {
            std::print(".");
        }
    }
    std::print("]");
    std::fflush(stdout);
}

void AccumulateFramebuffers(const auto& renderers, Framebuffer& outFramebuffer) {
    int usedRenderers = 0;

    for (const auto& renderer : renderers) {
        if (!renderer->WasUsed())
            continue;

        if (usedRenderers == 0) {
            renderer->GetFramebuffer(outFramebuffer);
        } else {
            Framebuffer tmp;
            renderer->GetFramebuffer(tmp);
            outFramebuffer.Add(tmp);
        }
        usedRenderers++;
    }

    if (usedRenderers > 0) {
        outFramebuffer.Scale(1.f / usedRenderers);
    }
}

// The main rendering function, renders what is in aConfig
auto render(const Config &aConfig) {
    // Set number of used threads
    omp_set_num_threads(aConfig.mNumThreads);

    // Create 1 renderer per thread
    std::vector<std::unique_ptr<AbstractRenderer> > renderers;
    renderers.reserve(aConfig.mNumThreads);

    for (int i = 0; i < renderers.capacity(); i++) {
        auto renderer = CreateRenderer(aConfig, aConfig.mBaseSeed + i);
        renderer->mMaxPathLength = aConfig.mMaxPathLength;
        renderer->mMinPathLength = aConfig.mMinPathLength;
        renderers.push_back(std::move(renderer));
    }

    auto startT = std::chrono::high_resolution_clock::now();
    int iter = 0;
    int globalCounter = 0;

    // Rendering loop
#pragma omp parallel for
    for (iter = 0; iter < aConfig.mIterations; iter++) {
        int threadId = omp_get_thread_num();
        renderers[threadId]->RunIteration(iter);

        // Print progress bar
#pragma omp critical
        {
            globalCounter++;
            PrintProgressBar(globalCounter, aConfig.mIterations);
        }
    }

    auto endT = std::chrono::high_resolution_clock::now();

    // Accumulate from all renderers into a common framebuffer
    AccumulateFramebuffers(renderers, *aConfig.mFramebuffer);

    // in seconds
    return std::chrono::duration_cast<std::chrono::milliseconds>(endT - startT).count() / 1000.f;
}

//////////////////////////////////////////////////////////////////////////
// Main

int main(int argc, const char *argv[]) {
    // Warns when not using C++11 Mersenne Twister
    PrintRngWarning();

    // Setups config based on command line
    Config config;
    ParseCommandline(argc, argv, config);

    // When some error has been encountered, exit
    if (config.mScene == nullptr)
        return 1;

    // Sets up framebuffer and number of threads
    Framebuffer fbuffer;
    config.mFramebuffer = &fbuffer;

    // Prints what we are doing
    std::println("Scene:     {}", config.mScene->mSceneName);
    std::println("Target:    {} iteration(s)", config.mIterations);

    // Renders the image
    std::println("Running ...");
    auto time = render(config);
    std::println("Done in {:.2f} s", time);

    // Saves the image
    const std::filesystem::path outputName = config.mOutputName;
    std::print("Saving to: {} ... ", outputName.string());
    const std::string &extension = config.mOutputName.substr(config.mOutputName.length() - 3, 3);
    fbuffer.Save(outputName, extension);
}
