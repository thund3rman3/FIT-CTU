#pragma once

/*
    AABB.h
    Axis-aligned bounding box definition and ray intersection support.
    Used to accelerate spatial queries during KD-tree traversal.
*/

#include <cmath>
#include <array>

#include "vec3.h"
#include "../utils/constants.h"
#include "../core/ray.h"

struct sAABB{
    vec3 _min;
    vec3 _max;

    /**
     * @brief Initialize the bounding box to an empty invalid region.
     */
    sAABB() : _min(vec3(INF)), _max(vec3(-INF)) {}

    /**
     * @brief Construct a bounding box containing the provided vertices.
     *
     * @param v Array of vertex positions.
     */
    sAABB(const std::array<vec3, DIM>& v);

    /**
     * @brief Expand the AABB to include the given point.
     *
     * @param point Point to include.
     */
    void update(const vec3& point) ;

    float surfaceArea() const {
        vec3 d = _max - _min;
        return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    /**
     * @brief Test ray intersection with the AABB and compute entry/exit distances.
     *
     * @param ray Ray to test.
     * @param tMin Output near intersection distance.
     * @param tMax Output far intersection distance.
     * @return True if the ray intersects the box.
     */
    bool intersectRay(const sRay& ray, float& tMin, float& tMax) const;
};

inline sAABB sceneAABB;