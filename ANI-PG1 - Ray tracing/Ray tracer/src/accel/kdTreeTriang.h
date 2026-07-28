#pragma once

/*
    kdTreeTriang.h
    KD-tree acceleration structure for triangle meshes, including SAH-based
    build logic and fast ray traversal for intersection queries.
*/

#include <vector>
#include <algorithm>
#include <chrono>

#include "../core/hit.h"
#include "../core/triangle.h"
#include "../math/vec3.h"
#include "../math/AABB.h"
#include "../utils/logger.h"
#include "../utils/utils.h"
#include "kdNode.h"
#include "kdHelper.h"

class cKdTreeTris {
public:

    /**
     * @brief Construct the KD-tree from a set of triangles.
     *
     * @param tris Triangle list to build into the acceleration structure.
     */
    cKdTreeTris(const std::vector<sTriangle>& tris);

    /**
     * @brief Traverse the KD-tree and find the nearest intersection.
     *
     * @param ray Ray to intersect with the scene.
     * @param cullback Enable backface culling.
     * @param toLight If true, use light-specific traversal.
     * @param dst Maximum distance to search.
     * @return Intersection hit information.
     */
    sHitInfo traverse(const sRay& ray, bool cullback, bool toLight = false, float dst = -INF) const {
        return traversePBRT(ray, cullback, toLight, dst);
    }
    
private:
    std::vector<sKDTreeNode> _treeNodes;
    std::vector<int> _leafTriangleIndices;
    const std::vector<sTriangle>* _triangles;
    static constexpr uint32_t MAX_TRIS_PER_LEAF = 4;
    static constexpr uint32_t MAX_DEPTH = 64;

    struct sStackElem {
        uint32_t _node;
        float _tMin;
        float _tMax;
    };

    /**
     * @brief Recursively build the KD-tree by splitting triangles.
     *
     * @param currIds Current triangle indices.
     * @param depth Current depth in the tree.
     * @return Node index in the KD-tree.
     */
    int buildRecursive(const std::vector<int>& currIds, uint32_t depth);

    /**
     * @brief Build the KD-tree using Surface Area Heuristic.
     *
     * @param currIds Triangle indices for this node.
     * @param nodeAABB Bounding box of the node.
     * @param depth Current node depth.
     * @return Node index in the KD-tree.
     */
    int buildTreeSAH(std::vector<int>& currIds, const sAABB& nodeAABB, uint32_t depth);

    /**
     * @brief Convert a triangle set into a leaf node.
     *
     * @param nodeIdx Index of the node to convert.
     * @param ids Triangle indices stored in the leaf.
     */
    void makeLeaf(uint32_t nodeIdx, const std::vector<int>& ids);

    /**
     * @brief Create a new leaf node and store the triangle indices.
     *
     * @param ids Triangle indices for the new leaf.
     * @return Index of the created leaf node.
     */
    uint32_t createLeaf(const std::vector<int>& ids);

    /**
     * @brief Perform PBRT-style KD-tree traversal for ray intersection.
     *
     * @param ray Ray to trace.
     * @param cullback Enable backface culling.
     * @param toLight Light traversal mode flag.
     * @param dst Maximum distance to search.
     * @return Intersection hit information.
     */
    sHitInfo traversePBRT(const sRay& ray, bool cullback, bool toLight = false, float dst = -INF) const;
};
