#pragma once

/*
    kdNode.h
    Represents a KD-tree node for triangle acceleration structure.
    Supports internal split nodes and leaf nodes storing triangle ranges.
*/

#include <cstdint>

#include "kdHelper.h"

struct sKDTreeNode{
    float splitPos; // position of the splitting plane along the chosen axis

    union {
        int leftIdx;     // internal node    
        int triStartIdx; // leaf node
    };
    union {
        uint32_t rightIdx;   // internal node
        uint32_t triCnt;     // leaf node
    };
    eAxis axis;

    sKDTreeNode()
        : splitPos(0.0f), triStartIdx(-1), triCnt(0), axis(eAxis::None) {}

    sKDTreeNode(eAxis ax, float split, int tri, uint32_t triCount = 1)
        : splitPos(split), triStartIdx(tri), triCnt(triCount), axis(ax) {}

    bool isLeaf() const { return axis == eAxis::None; }
};