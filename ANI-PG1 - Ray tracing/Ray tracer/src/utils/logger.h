#pragma once

/*
    logger.h
    Collects runtime statistics for the renderer and acceleration structures.
    Tracks triangle counts, traversal timings, mipmap usage, and memory details.
*/

#include <limits>
#include <chrono>
#include <fstream>
#include <ios>

#include "constants.h"
#include "cfgLoader.h"
#include "../math/AABB.h"


struct sLogger{
    size_t _cntTriangles = 0;
    size_t _cntMaterials = 0;
    size_t _cntLights = 0;

    size_t _cntPrim = 0;
    size_t _cntRefl = 0;
    uint32_t _cntRefr = 0;
    uint32_t _cntShad = 0;
    size_t _cntIntersect = 0;

    // texel level for mipmapping
    float _maxMipLvl = 0.0f; // max level of mipmap used in render
    float _minMipLvl = INF;   // min level of mipmap used in render
    float _maxTexSize = 0.0f; // max texel size used in render
    float _minTexSize = INF; // min texel size used in render

    // incidence in traversal
    uint32_t _minCntIntersect = std::numeric_limits<uint32_t>::max();
    uint32_t _maxCntIntersect = 0;
    float _avgCntIntersect = 0.0f;
    uint32_t _cntIntersectPerTraversal = 0;
    uint32_t _cntTraversals = 0;

    // build, traverse time
    std::chrono::duration<float, std::milli> _buildTime;
    std::chrono::duration<float, std::milli> _minRayTimeTraversal = std::chrono::duration<float, std::milli>(INF);
    std::chrono::duration<float, std::milli> _maxRayTimeTraversal = std::chrono::duration<float, std::milli>(0.0f);
    std::chrono::duration<float, std::milli> _avgRayTimePerTraversal;
    std::chrono::duration<float, std::milli> _avgShadowRayTimePerTraversal;
    float _avgRayCountPerTraversal = 0;
    float _avgShadowRayCountPerTraversal = 0;

    // traversal iterations count
    uint32_t _minCntTreverseIterations = std::numeric_limits<uint32_t>::max();
    uint32_t _maxCntTraverseIterations = 0;
    float _avgCntTraverseIterations = 0.0f;
    uint32_t _cntTreversalIterations = 0;

    // depth of traversal
    uint32_t _maxTraversalDepth = 0;
    uint32_t _minTraversalDepth = std::numeric_limits<uint32_t>::max();
    float _avgTraversalDepth = 0.0f;
    uint32_t _traversalDepth = 0;

    // build
    uint32_t _cntVertices = 0;
    uint32_t _cntLeaf = 0;
    uint32_t _maxDepth = 0;
    uint32_t _minDepth = std::numeric_limits<uint32_t>::max();
    float _avgDepth = 0;
    uint32_t _maxTrisPerLeaf = 0;
    uint32_t _minTrisPerLeaf = std::numeric_limits<uint32_t>::max();
    uint32_t _avgTrisPerLeaf = 0;
    uint32_t _memUsage = 0;

    /**
     * @brief Save collected render and acceleration statistics to a log file.
     *
     * @param start Start time of rendering to compute total duration.
     */
    void saveToLog(const std::chrono::steady_clock::time_point& start);

    /**
     * @brief Record statistics for a single ray intersection event.
     *
     * @param start Start time of ray traversal.
     * @param isPrimary True if the ray is a primary camera ray.
     */
    void logRayIntersection(const std::chrono::steady_clock::time_point& start, bool isPrimary);
};

inline sLogger logger;