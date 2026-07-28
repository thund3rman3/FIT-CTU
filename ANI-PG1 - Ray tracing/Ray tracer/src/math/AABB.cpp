#include "AABB.h"

sAABB::sAABB(const std::array<vec3, DIM>& v) {
    _min.x = std::min({v[0].x, v[1].x, v[2].x});
    _min.y = std::min({v[0].y, v[1].y, v[2].y});
    _min.z = std::min({v[0].z, v[1].z, v[2].z});
    _max.x = std::max({v[0].x, v[1].x, v[2].x});
    _max.y = std::max({v[0].y, v[1].y, v[2].y});
    _max.z = std::max({v[0].z, v[1].z, v[2].z});
}

void sAABB::update(const vec3& point) {
    _min.x = std::min(_min.x, point.x);
    _min.y = std::min(_min.y, point.y);
    _min.z = std::min(_min.z, point.z);
    _max.x = std::max(_max.x, point.x);
    _max.y = std::max(_max.y, point.y);
    _max.z = std::max(_max.z, point.z);
}

bool sAABB::intersectRay(const sRay& ray, float& tMin, float& tMax) const {
    vec3 invDir(vec3(1.0f) / vec3(ray._dir.x, ray._dir.y, ray._dir.z));

    // Find for X
    float txMin = (_min.x - ray._origin.x) * invDir.x;
    float txMax = (_max.x - ray._origin.x) * invDir.x;

    tMin = std::min(txMin, txMax);
    tMax = std::max(txMin, txMax);

    // Find for Y
    float tyMin = (_min.y - ray._origin.y) * invDir.y;
    float tyMax = (_max.y - ray._origin.y) * invDir.y;

    tMin = std::max(tMin, std::min(tyMin, tyMax));
    tMax = std::min(tMax, std::max(tyMin, tyMax));

    // Find for Z
    float tzMin = (_min.z - ray._origin.z) * invDir.z;
    float tzMax = (_max.z - ray._origin.z) * invDir.z;

    tMin = std::max(tMin, std::min(tzMin, tzMax));
    tMax = std::min(tMax, std::max(tzMin, tzMax));

    // AABB intersection is valid if:
    // 1. t_max >= t_min (otherwise the ray missed the box).
    // 2. t_max >= zero (otherwise the entire box is behind the ray).
    return tMax >= std::max(0.0f, tMin);
}

