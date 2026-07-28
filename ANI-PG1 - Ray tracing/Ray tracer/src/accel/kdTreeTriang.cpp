#include "kdTreeTriang.h"


cKdTreeTris::cKdTreeTris(const std::vector<sTriangle>& tris){
    _triangles = &tris;

    std::vector<int> initialIndices(_triangles->size());
    for (uint32_t i = 0; i < _triangles->size(); ++i) {
        initialIndices[i] = i;
    }
    _leafTriangleIndices.reserve(_triangles->size() * 2);
    _treeNodes.reserve(_triangles->size() * MAX_TRIS_PER_LEAF);

    auto start = std::chrono::steady_clock::now();
    //buildRecursive(initialIndices, 0);
    buildTreeSAH(initialIndices, sceneAABB, 0);

    auto end = std::chrono::steady_clock::now();
    logger._buildTime = end - start;
    logger._memUsage = static_cast<int>(_treeNodes.size() * sizeof(sKDTreeNode) + static_cast<int>(_leafTriangleIndices.size()) * sizeof(int));

    _treeNodes.shrink_to_fit(); 
    _leafTriangleIndices.shrink_to_fit();
}

int cKdTreeTris::buildRecursive(const std::vector<int>& currIds, uint32_t depth) {
    uint32_t count = static_cast<int>(currIds.size());

    if (count == 0) 
        return -1;

    if (count <= MAX_TRIS_PER_LEAF || depth > MAX_DEPTH){
        logger._maxDepth = std::max(logger._maxDepth, depth);
        logger._minDepth = std::min(logger._minDepth, depth);
        logger._avgDepth += depth;
        return createLeaf(currIds);
    }

    uint32_t axisIdx = depth % 3;
    eAxis axis = static_cast<eAxis>(axisIdx);

    float minPos = INF;
    float maxPos = std::numeric_limits<float>::lowest();
    
    // Min and max of the bounding box for current set of triangles along the chosen axis
    for (uint32_t idx : currIds) {
        const sAABB& box = (*_triangles)[idx]._aabb; 
        
        float bMin = (axisIdx == 0) ? box._min.x : ((axisIdx == 1) ? box._min.y : box._min.z);
        float bMax = (axisIdx == 0) ? box._max.x : ((axisIdx == 1) ? box._max.y : box._max.z);
        
        if (bMin < minPos) 
            minPos = bMin;
        if (bMax > maxPos) 
        maxPos = bMax;
    }
    
    float split_pos = minPos + (maxPos - minPos) * 0.5f;

    std::vector<int> leftIds;
    std::vector<int> rightIds;
    
    leftIds.reserve(count);
    rightIds.reserve(count);

    // Partition triangles based on their bounding boxes relative to the splitting plane
    for (uint32_t idx : currIds) {
        const sAABB& box = (*_triangles)[idx]._aabb;
        
        float bMin = (axisIdx == 0) ? box._min.x : ((axisIdx == 1) ? box._min.y : box._min.z);
        float bMax = (axisIdx == 0) ? box._max.x : ((axisIdx == 1) ? box._max.y : box._max.z);

        if (bMax <= split_pos) {
            leftIds.push_back(idx);
        } 
        else if (bMin >= split_pos) {
            rightIds.push_back(idx);
        } 
        else {
            leftIds.push_back(idx);
            rightIds.push_back(idx);
        }
    }

    // To prevent infinite recursion in cases where all triangles overlap the splitting plane
    // create a leaf node from all of them.
    if (leftIds.size() == static_cast<size_t>(count) && rightIds.size() == static_cast<size_t>(count)) {
        return createLeaf(currIds);
    }

    uint32_t nodeIdx = static_cast<int>(_treeNodes.size());
    _treeNodes.emplace_back(axis, split_pos, -1, -1);
    ++logger._cntVertices;
    _treeNodes[nodeIdx].leftIdx = buildRecursive(leftIds, depth + 1);;
    _treeNodes[nodeIdx].rightIdx = buildRecursive(rightIds, depth + 1);;

    return nodeIdx;
}

int cKdTreeTris::buildTreeSAH(std::vector<int>& currIds, const sAABB& nodeAABB, uint32_t depth) {
    int nodeIdx = static_cast<int>(_treeNodes.size());
    _treeNodes.push_back(sKDTreeNode());
    ++logger._cntVertices;
    
    uint32_t numTris = static_cast<int>(currIds.size());

    if (numTris <= MAX_TRIS_PER_LEAF || depth >= MAX_DEPTH) {
        logger._maxDepth = std::max(logger._maxDepth, depth);
        logger._minDepth = std::min(logger._minDepth, depth);
        logger._avgDepth += depth;
        makeLeaf(nodeIdx, currIds);
        return nodeIdx;
    }

    float emptyBonus = 0.5f; // PBRT trick: Bonus for empty space
    float isectCost = 80.0f; // Cost of intersection with triangle (splitting is expensive!)
    float travCost = 1.0f;   // Cost of node traversal

    float bestCost = INF;
    int bestAxis = -1;
    float bestSplitPos = 0.0f;

    float invTotalArea = 1.0f / nodeAABB.surfaceArea();
    std::vector<sBoundEdge> edges;
    edges.reserve(2 * numTris);

    // 2. Test all 3 axes (X, Y, Z)
    for (uint32_t axis = 0; axis < 3; ++axis) {
        edges.clear();
        
        // Create edges for all triangles
        for (uint32_t triId : currIds) {
            sAABB triBox = (*_triangles)[triId]._aabb;
            
            // Clipping the triangle's AABB to the current node (important for accuracy)
            triBox._min[axis] = std::max(triBox._min[axis], nodeAABB._min[axis]);
            triBox._max[axis] = std::min(triBox._max[axis], nodeAABB._max[axis]);

            if (triBox._min[axis] == triBox._max[axis]) {
                edges.emplace_back(triBox._min[axis], triId, eEdgeType::PLANAR);
            } else {
                edges.emplace_back(triBox._min[axis], triId, eEdgeType::START);
                edges.emplace_back(triBox._max[axis], triId, eEdgeType::END);
            }
        }

        // Sort edges by position along the axis
        std::sort(edges.begin(), edges.end());

        uint32_t nBelow = 0;
        uint32_t nAbove = numTris;

        // 3. Sweep Line
        for (size_t i = 0; i < edges.size(); ++i) {
            if (edges[i].type == eEdgeType::END) nAbove--;
            if (edges[i].type == eEdgeType::PLANAR) nAbove--;

            float splitPos = edges[i].t;

            // Calculate the surface area of potential children (if we were to split here)
            if (splitPos > nodeAABB._min[axis] && splitPos < nodeAABB._max[axis]) {
                
                // Preliminary AABB calculation for both children
                sAABB leftAABB = nodeAABB;  leftAABB._max[axis] = splitPos;
                sAABB rightAABB = nodeAABB; rightAABB._min[axis] = splitPos;

                float pBelow = leftAABB.surfaceArea() * invTotalArea;
                float pAbove = rightAABB.surfaceArea() * invTotalArea;

                // Bonus for empty space
                float eb = (nAbove == 0 || nBelow == 0) ? emptyBonus : 0.0f;

                // SAH cost function: Cost = TraversalCost + IntersectionCost * (1 - EmptyBonus) * (ProbBelow * NumBelow + ProbAbove * NumAbove)
                float cost = travCost + isectCost * (1.0f - eb) * (pBelow * nBelow + pAbove * nAbove);

                if (cost < bestCost) {
                    bestCost = cost;
                    bestAxis = axis;
                    bestSplitPos = splitPos;
                }
            }

            if (edges[i].type == eEdgeType::START) nBelow++;
            if (edges[i].type == eEdgeType::PLANAR) nBelow++;
        }
    }

    // 4. Decision: Is it worth splitting at all?
    // If the cost of splitting is higher than simply testing all triangles,
    // or if we couldn't find a valid split
    float leafCost = isectCost * numTris;
    if (bestCost > leafCost || bestAxis == -1) {
        logger._maxDepth = std::max(logger._maxDepth, depth);
        logger._minDepth = std::min(logger._minDepth, depth);
        logger._avgDepth += depth;
        makeLeaf(nodeIdx, currIds);
        return nodeIdx;
    }

    // 5. We found the best split! Physically divide the triangles into left and right nodes
    std::vector<int> leftChildTris, rightChildTris;
    for (uint32_t triId : currIds) {
        sAABB triBox = (*_triangles)[triId]._aabb;
        
        // Put the triangle to the left if its start is before the splitting plane
        if (triBox._min[bestAxis] <= bestSplitPos) 
            leftChildTris.push_back(triId);
        // Put it to the right if its end extends beyond the splitting plane
        if (triBox._max[bestAxis] >= bestSplitPos) 
            rightChildTris.push_back(triId);
    }

    // Free memory of current node array
    currIds.clear(); 
    currIds.shrink_to_fit();

    // 6. Set properties of the internal node
    _treeNodes[nodeIdx].axis = static_cast<eAxis>(bestAxis);
    _treeNodes[nodeIdx].splitPos = bestSplitPos;
    _treeNodes[nodeIdx].triCnt = 0; // 0 znamená, že to NENÍ list

    sAABB leftAABB = nodeAABB; 
    leftAABB._max[bestAxis] = bestSplitPos;
    _treeNodes[nodeIdx].leftIdx = buildTreeSAH(leftChildTris, leftAABB, depth + 1);

    sAABB rightAABB = nodeAABB; 
    rightAABB._min[bestAxis] = bestSplitPos;
    _treeNodes[nodeIdx].rightIdx = buildTreeSAH(rightChildTris, rightAABB, depth + 1);

    return nodeIdx;
}

void cKdTreeTris::makeLeaf(uint32_t nodeIdx, const std::vector<int>& ids) {
    _treeNodes[nodeIdx].axis = eAxis::None;
    _treeNodes[nodeIdx].triCnt = static_cast<int>(ids.size());
    _treeNodes[nodeIdx].triStartIdx = static_cast<int>(_leafTriangleIndices.size());
    
    for (uint32_t id : ids) {
        _leafTriangleIndices.push_back(id);
    }

    logger._cntLeaf++;
    logger._maxTrisPerLeaf = std::max(logger._maxTrisPerLeaf, _treeNodes[nodeIdx].triCnt);
    logger._minTrisPerLeaf = std::min(logger._minTrisPerLeaf, _treeNodes[nodeIdx].triCnt);
    logger._avgTrisPerLeaf += _treeNodes[nodeIdx].triCnt;
}

uint32_t cKdTreeTris::createLeaf(const std::vector<int>& ids) {
    uint32_t leafIdx = static_cast<int>(_treeNodes.size());
    _treeNodes.emplace_back(eAxis::None, 0.0f, -1, -1);
    
    _treeNodes[leafIdx].triStartIdx = static_cast<int>(_leafTriangleIndices.size()); 
    _treeNodes[leafIdx].triCnt = static_cast<int>(ids.size());
    
    _leafTriangleIndices.insert(_leafTriangleIndices.end(), ids.begin(), ids.end());
    
    logger._cntLeaf++;
    logger._maxTrisPerLeaf = std::max(logger._maxTrisPerLeaf, _treeNodes[leafIdx].triCnt);
    logger._minTrisPerLeaf = std::min(logger._minTrisPerLeaf, _treeNodes[leafIdx].triCnt);
    logger._avgTrisPerLeaf += _treeNodes[leafIdx].triCnt;

    return leafIdx;
}

sHitInfo cKdTreeTris::traversePBRT(const sRay& ray, bool cullback, bool toLight, float dst) const {
    float t_min, t_max;
    sHitInfo hitInfo;
    ++logger._cntTraversals;
    
    if (!sceneAABB.intersectRay(ray, t_min, t_max)) {
        return hitInfo; 
    }

    if (t_min < 0.0f) 
        t_min = 0.0f;

    std::array<sStackElem, MAX_DEPTH> stack;
    uint32_t stackPtr = 0;

    stack[stackPtr++] = {0, t_min, t_max};

    while (stackPtr > 0) {
        --stackPtr;
        uint32_t currNode = stack[stackPtr]._node;
        float tMin = stack[stackPtr]._tMin;
        float tMax = stack[stackPtr]._tMax;

        ++logger._cntTreversalIterations;
        while (!_treeNodes[currNode].isLeaf()) {
            
            float splitVal = _treeNodes[currNode].splitPos;
            uint32_t axis = static_cast<int>(_treeNodes[currNode].axis);
            
            uint32_t first, second;
            if (ray._dir[axis] >= 0.0f) {
                first = _treeNodes[currNode].leftIdx;
                second = _treeNodes[currNode].rightIdx;
            } else {
                first = _treeNodes[currNode].rightIdx;
                second = _treeNodes[currNode].leftIdx;
            }

            float tSplit = (splitVal - ray._origin[axis]) * ray._invDir[axis];

            if (tMax <= tSplit) {
                currNode = first;
            } 
            else if (tSplit <= tMin) {
                currNode = second;
            } 
            else {
                stack[stackPtr]._node = second;
                stack[stackPtr]._tMin = tSplit;
                stack[stackPtr]._tMax = tMax;
                stackPtr++;

                currNode = first;
                tMax = tSplit;
                if (stackPtr > logger._traversalDepth)
                    logger._traversalDepth = stackPtr;
            }
            ++logger._cntTreversalIterations;
        }

        for (uint32_t i = 0; i < _treeNodes[currNode].triCnt; ++i) {
            uint32_t triId = _leafTriangleIndices[_treeNodes[currNode].triStartIdx + i];
            const sTriangle& tri = (*_triangles)[triId];
            float b1, b2;

            float t = intersect(ray, tri, cullback, b1, b2);
            ++logger._cntIntersect;
            ++logger._cntIntersectPerTraversal;

            if (((toLight && t < dst) || (!toLight && t < hitInfo._t )) && t > fEpsilon) {
                hitInfo._t = t;
                hitInfo._hitTri = &tri;
                hitInfo._b1 = b1;
                hitInfo._b2 = b2;
                
                if(toLight) 
                    return hitInfo; 
            }
        }

        if (!toLight && hitInfo.isHit() && hitInfo._t <= tMax + 0.0001f)
            return hitInfo; 
    }

    return hitInfo; // No hit, hitTri will be nullptr
}