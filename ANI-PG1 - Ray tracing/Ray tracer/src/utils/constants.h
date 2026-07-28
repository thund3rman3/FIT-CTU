#pragma once

/*
    constants.h
    Global compile-time constants and application-wide default values
    used throughout the ray tracer.
*/

#include <limits>
#include <string>

inline constexpr uint32_t DIM = 3; 
inline constexpr float INF = std::numeric_limits<float>::infinity();
inline constexpr float PI = 3.14159265358979323846f;
inline constexpr float gEpsilon = 1.0e-5f; // offset from geometry
inline constexpr float lEpsilon = 1.0e-2f; // offset from light 
inline constexpr float fEpsilon = 1.0e-9f; // for comparing floats, and for ray-box intersection to avoid division by zero
inline const std::string CONFIG_PATH = "config.json";