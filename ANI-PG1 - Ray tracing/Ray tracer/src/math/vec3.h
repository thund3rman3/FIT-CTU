#pragma once

/*
    vec3.h
    A 3D vector utility type with arithmetic operators and geometry helpers.
    Used for positions, directions, normals, and colors in the ray tracer.
*/

#include <ostream>
#include <cmath>

#include "../utils/constants.h"

struct vec3 {
    float x, y, z;

    vec3() 
        : x(0.0f), y(0.0f), z(0.0f) {}
    vec3(float v) 
        : x(v), y(v), z(v) {}
    vec3(float x, float y, float z = 0) 
        : x(x), y(y), z(z) {}

    vec3 operator*(float a) const { 
        return vec3(x * a, y * a, z * a); 
    }
    vec3 operator*(const vec3 r) const { 
        return vec3(x * r.x, y * r.y, z * r.z); 
    }
    vec3 operator/(const float r) const { 
        return std::abs(r) > fEpsilon ? vec3(x / r, y / r, z / r) : vec3(0, 0, 0); 
    }
    vec3 operator+(const vec3& v) const { 
        return vec3(x + v.x, y + v.y, z + v.z); 
    }
    vec3 operator-(const vec3& v) const { 
        return vec3(x - v.x, y - v.y, z - v.z); 
    }
    vec3 operator-() const { 
        return vec3(-x, -y, -z); 
    }
    void operator+=(const vec3& v) { 
        x += v.x, y += v.y, z += v.z; 
    }
    void operator*=(float a) { 
        x *= a, y *= a, z *= a; 
    }
    void operator*=(const vec3& v) { 
        x *= v.x, y *= v.y, z *= v.z; 
    }

    float& operator[](uint32_t idx) { 
        return (idx == 0) ? x : ((idx == 1) ? y : z); 
    }
    const float& operator[](uint32_t idx) const { 
        return (idx == 0) ? x : ((idx == 1) ? y : z); 
    }
    bool operator==(const vec3& other) const {
        return std::abs(x - other.x) < fEpsilon &&
                std::abs(y - other.y) < fEpsilon && 
                std::abs(z - other.z) < fEpsilon;
    }
    vec3 operator/(const vec3& other) const {
        return vec3(
            std::abs(other.x) > fEpsilon ? x / other.x : 0.0f,
            std::abs(other.y) > fEpsilon ? y / other.y : 0.0f,
            std::abs(other.z) > fEpsilon ? z / other.z : 0.0f
        );
    }

    float length() const { return std::sqrt(x * x + y * y + z * z); }
    float average() { return (x + y + z) / 3; }
    vec3 normalize() const { return (*this) / length(); }
};

inline float dot(const vec3& v1, const vec3& v2) { 
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; 
}

inline vec3 cross(const vec3& v1, const vec3& v2) {
    return vec3(v1.y * v2.z - v1.z * v2.y,
                 v1.z * v2.x - v1.x * v2.z, 
                 v1.x * v2.y - v1.y * v2.x);
}

inline float dst(const vec3& v1, const vec3& v2){
    vec3 diff = v1 - v2;
    return diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
}

inline std::ostream& operator<<(std::ostream& os, const vec3& vec) {
    os << vec.x << " " << vec.y << " " << vec.z;
    return os;
}