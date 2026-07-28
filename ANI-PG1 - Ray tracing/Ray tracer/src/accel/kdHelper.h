#pragma once

/*
    kdHelper.h
    Helper enums and edge structures used for KD-tree construction
    using surface area heuristics and triangle splitting.
*/

#include <cstdint>

enum class eAxis : uint8_t { 
    X = 0, 
    Y = 1, 
    Z = 2, 
    None = 3 // leaf node indicator
};

enum class eEdgeType : uint8_t {
    START = 0,    // Tri begins here
    PLANAR = 1, // Tri lies in split plane
    END = 2  // Tri ends here
};

struct sBoundEdge {
    float t;         
    uint32_t triId;     
    eEdgeType type;   

    bool operator<(const sBoundEdge& other) const {
        if (t == other.t) 
            return static_cast<int>(type) < static_cast<int>(other.type);
        return t < other.t;
    }
};