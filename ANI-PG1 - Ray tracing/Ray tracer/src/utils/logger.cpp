#include "logger.h"

void sLogger::logRayIntersection(const std::chrono::steady_clock::time_point& start, bool isPrimary){
    auto end = std::chrono::steady_clock::now();

    _minCntTreverseIterations = std::min(_minCntTreverseIterations, _cntTreversalIterations);
    _maxCntTraverseIterations = std::max(_maxCntTraverseIterations, _cntTreversalIterations);
    _avgCntTraverseIterations += _cntTreversalIterations;
    _cntTreversalIterations = 0; 

    _minTraversalDepth = std::min(_minTraversalDepth, _traversalDepth);
    _maxTraversalDepth = std::max(_maxTraversalDepth, _traversalDepth);
    _avgTraversalDepth += _traversalDepth;
    _traversalDepth = 0;

    _minCntIntersect = std::min(_cntIntersectPerTraversal, _minCntIntersect);
    _maxCntIntersect = std::max(_cntIntersectPerTraversal, _maxCntIntersect);
    _avgCntIntersect += _cntIntersectPerTraversal;
    _cntIntersectPerTraversal = 0; 
    
    std::chrono::duration<float, std::milli> time = end - start;
    _minRayTimeTraversal = std::min(_minRayTimeTraversal, time);
    _maxRayTimeTraversal = std::max(_maxRayTimeTraversal, time);
    
    if(isPrimary) {
        _avgRayTimePerTraversal += time;
        _avgRayCountPerTraversal += 1.0f;
    } else {
        _avgShadowRayTimePerTraversal += time;
        _avgShadowRayCountPerTraversal += 1.0f;
    }
}

void sLogger::saveToLog(const std::chrono::steady_clock::time_point& start){

    std::ofstream report("output/times.log", std::ios::app);
    if(report.is_open()){
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<float, std::milli> duration = end - start;

        report << cfg.MAT_PATH << "; GGX:" << cfg.GGX << std::endl;
        
        report << "\t Time: " << duration.count() << " ms."<< std::endl;
        report << "Mip level: (" << logger._minMipLvl << ", " << logger._maxMipLvl << ")" << " TexScale: (" << logger._minTexSize << ", " << logger._maxTexSize << ")" << std::endl;
        report << "\t Scene AABB: Min(" << sceneAABB._min.x << ", " << sceneAABB._min.y << ", " << sceneAABB._min.z << ") Max(" 
                << sceneAABB._max.x << ", " << sceneAABB._max.y << ", " << sceneAABB._max.z << ")" << std::endl;
        report << "\t Loaded " << _cntTriangles << " triangles, " << _cntMaterials << " materials, " << _cntLights << " light sources." << std::endl;
        
        report << "\t Ray tracing stats: " << std::endl;
        report << "\t\t Depth: " << cfg.REC_DEPTH_MAX 
                << "; AL samples: " << cfg.LIGHT_SAMPLES << std::endl; 
        report << "\t\t Rays: " << (_cntPrim + _cntRefl + _cntRefr + _cntShad) / (duration.count() / 1000) / 1000 
                << "kRays/s; INTERSECTIONS tried: " << _cntIntersect << std::endl;
        report << "\t\t Prim / Refl / Refr / Shad rays: " << _cntPrim << " / " << _cntRefl << " / " << _cntRefr << " / " << _cntShad << std::endl;
        
        float safeLeafCnt = (_cntLeaf > 0) ? static_cast<float>(_cntLeaf) : 1.0f;
        report << "\t KD-Tree build stats: " << std::endl;
        report << "\t\t Build time: " << _buildTime.count() << " ms; Number of nodes: " << _cntVertices 
                << " (" << _cntVertices - _cntLeaf << " internal, " << _cntLeaf << " leaf) " << std::endl;
        report << "\t\t Depth: min " << _minDepth 
                << "; max " << _maxDepth 
                << "; avg " << _avgDepth / safeLeafCnt << std::endl;
        report << "\t\t Tris per leaf: min " << _minTrisPerLeaf 
                << "; max " << _maxTrisPerLeaf 
                << "; avg " << _avgTrisPerLeaf / safeLeafCnt << std::endl;
        report << "\t\t Memory usage: " << _memUsage / 1024.0f << " KB " << std::endl;
        

        float safeTraverseCnt = (_cntTraversals > 0) ? static_cast<float>(_cntTraversals) : 1.0f;
        float safeRayCount = (_avgRayCountPerTraversal > 0.0f) ? _avgRayCountPerTraversal : 1.0f;
        float safeShadowRayCount = (_avgShadowRayCountPerTraversal > 0.0f) ? _avgShadowRayCountPerTraversal : 1.0f;
        report << "\t KD-Tree traversal stats: " << std::endl;
        report << "\t\t Incidences in one traversal - min: " << _minCntIntersect 
                << "; max: " << _maxCntIntersect 
                << "; avg: " << _avgCntIntersect / safeTraverseCnt << std::endl;
        report << "\t\t Traversal operations count in one traversal - min: " << _minCntTreverseIterations 
                << "; max " << _maxCntTraverseIterations 
                << "; avg " << _avgCntTraverseIterations / safeTraverseCnt << std::endl;
        report << "\t\t Avg ray traverse time: " << (_avgRayTimePerTraversal.count() / safeRayCount)
                    << " ms; Avg shadow ray traverse time: " << (_avgShadowRayTimePerTraversal.count() / safeShadowRayCount) << " ms." << std::endl;
        report << "\t\t Min ray traverse time: " << _minRayTimeTraversal.count() 
                << " ms; Max ray traverse time: " << _maxRayTimeTraversal.count() << " ms." << std::endl;
        report << "\t\t Depth of traversal - min: " << _minTraversalDepth 
                << "; max: " << _maxTraversalDepth 
                << "; avg: " << _avgTraversalDepth / safeTraverseCnt << std::endl;
        
        report.close();
    }
    else
        std::cerr << "Didn't log" << std::endl;
}