#pragma once

/*
    triangle.h
    Triangle primitive representation, including vertices, normals, UVs,
    tangents, bounding box, material index, and texture scaling.
*/

#include <array>

#include "../math/vec3.h"
#include "../utils/constants.h"
#include "../math/AABB.h"

struct sTriangle {
    std::array<vec3, DIM> _vertices;
    std::array<vec3, DIM> _normals;
    std::array<vec3, DIM> _uvs; //#TODO vec2
    std::array<vec3, DIM> _tangents;
    sAABB _aabb;
    vec3 _centeroid;
    uint32_t _matIdx;
    float _texScale; // for mipmapping
};

struct sTriangleLight {
    std::array<vec3, DIM> _vertices;
    vec3 _faceNormal;
    vec3 _emission;
    float _triArea;

    sTriangleLight(const std::array<vec3, DIM>& v, const vec3& n, const vec3& Ie, const float area)
        : _vertices(v), _faceNormal(n), _emission(Ie), _triArea(area){}
};