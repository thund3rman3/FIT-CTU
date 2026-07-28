#pragma once

/*
    ray.h
    Defines the ray structure used for tracing through the scene.
    Stores origin, direction, and inverse direction for intersection tests.
*/

#include "../math/vec3.h"

struct sRay {
    vec3 _origin;
    vec3 _dir;
    vec3 _invDir;

    sRay(const vec3& origin, const vec3& dir)
        : _origin(origin), _dir(dir), _invDir(vec3(1.0f) / vec3(dir.x, dir.y, dir.z)){}
};