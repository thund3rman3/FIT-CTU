#pragma once

/*
    camera.h
    Camera definition and ray generation support for perspective rendering.
    Computes image plane dimensions and step vectors from FOV and resolution.
*/

#include <cmath>

#include "../math/vec3.h"

struct sCamera{
    vec3 _pos; // Camera position
    vec3 _up; // Up vector for the camera
    vec3 _dir; // Forward vector for the camera
    vec3 _right; // Right vector for the camera
    vec3 _stepRight; // Step vector for moving right across the image plane
    vec3 _stepUp; // Step vector for moving up across the image plane (negative because y goes down in image space)
    vec3 _startPvec; // Starting point for the ray tracing (top-left corner of the image plane)
    float _fov; // field of view in radians
    float _gw; // real width in world coordinates
    float _gh; //real height in world coordinates
 
    sCamera(const vec3& pos, const vec3& up, const vec3& dir, float fov, uint32_t width, uint32_t height) 
        : _pos(pos), _up(up), _dir(dir), _fov(fov) {
        _right = cross(dir, up).normalize();
        _gw = 2.0f * std::tan(_fov * 0.5f); 
        _gh = _gw * (float)height / width;
        _stepRight = _right * _gw / (width - 1.0f);
        _stepUp = _up * (-1.0f) * _gh / (height - 1.0f);
        _startPvec = _dir - _right * (_gw * 0.5f) + _up * (_gh * 0.5f);
    }

    sCamera() = default;
        
};

// sCamera camera({278, 273, -1000}, {0, 1, 0}, {0, 0, 1}, 0.6f);
//sCamera camera({0.0f, 1.0f, 4.42f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, 0.6f);
inline sCamera camera;
/*
"CAMERA": {
        "pos": [0.0, 1.0, 4.42],
        "up":  [0.0, 1.0, 0.0],
        "dir": [0.0, 0.0, -1.0],
        "fov": 0.6
    },
"CAMERA": {
    "pos": [278, 273, -1000],
    "up":  [0.0, 1.0, 0.0],
    "dir": [0.0, 0.0, 1.0],
    "fov": 0.6
},
*/