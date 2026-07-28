#pragma once

/*
    light.h
    Represents a point or area light source using a position, color,
    and referenced emitting triangle index.
*/

#include "../math/vec3.h"

struct sLight{
    vec3 _pos;
    vec3 _col;
    uint32_t _triIdx; // idx to array of triangles on which this light is emitted from, for direct lighting sampling

    sLight(vec3& pos, vec3& col, uint32_t idx) 
        : _pos(pos), _col(col), _triIdx(idx) {}
};