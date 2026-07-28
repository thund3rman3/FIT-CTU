#pragma once

/*
    hit.h
    Stores ray-triangle intersection results, including hit distance
    parameters and barycentric coordinates.
*/

#include "triangle.h"
#include "../utils/cfgLoader.h"


struct sHitInfo {
    const sTriangle* _hitTri; // hit triangle pointer, if nullptr - no hit
    float _t; // hitpoint distance parameter                 
    float _b1; // barycentric coord
    float _b2; // barycentric coord

    sHitInfo()
        : _hitTri(nullptr), _t(cfg.DIST){}

    bool isHit() const { 
        return _hitTri != nullptr; 
    }
};




