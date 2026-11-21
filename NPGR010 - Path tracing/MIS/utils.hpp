#pragma once

#include <vector>
#include <cmath>
#include <utility>
#include "math.hpp"
#include "rng.hpp"

#define EPSILON_COSINE 1e-6f
#define EPSILON_RAY    1e-3f

// sRGB luminance
inline float Luminance(const Vec3f& aRGB)
{
    return 0.212671f * aRGB.x + 0.715160f * aRGB.y + 0.072169f * aRGB.z;
}

// reflect vector through (0,0,1)
inline Vec3f ReflectLocal(const Vec3f& aVector)
{
    return {-aVector.x, -aVector.y, aVector.z};
}

//////////////////////////////////////////////////////////////////////////
// Utilities for converting PDF between Area (A) and Solid angle (W)
// WtoA = PdfW * cosine / distance_squared
// AtoW = PdfA * distance_squared / cosine

inline float PdfWtoA(const float aPdfW, const float aDist, const float aCosThere)
{
    return aPdfW * std::abs(aCosThere) / Sqr(aDist);
}

inline float PdfAtoW(const float aPdfA, const float aDist, const float aCosThere)
{
    return aPdfA * Sqr(aDist) / std::abs(aCosThere);
}

inline Vec3f SampleHemisphereUniform(Rng& rng){
    // Uniform hemispherical sampling

    // Height cos(theta), uniform in <0; 1).
    // Solid angle element is dw = sin(theta) dtheta dphi = dz dphi, so uniform
    // cosTheta and uniform phi give uniform directions
    float cosTheta = rng.GetFloat(); // sample height

    // Azimuth angle around Z (Up/Normal) Axis from <0;2PI)
    float phi = 2.0f * PI_F * rng.GetFloat();

    // Radius of the circle at height z on the unit sphere (Pythagoras):
    // r^2 + cosTheta^2 = 1  ->  r = sqrt(1 - cosTheta^2)
    float r = std::sqrt(std::max(0.0f, 1.0f - cosTheta * cosTheta));
    return {r * std::cos(phi), r * std::sin(phi), cosTheta};
}

inline Vec3f SampleHemisphereCos(Rng& rng)
{
    // Hemispherical sampling proportional to cos(theta)

    // Azimuth angle around Z-Up-Axis from <0;2PI)
    float phi = 2.0f * PI_F * rng.GetFloat();

    /** Unit sphere -> radius=1 -> maxHeight = 1 = cos(Theta)
    Theta is angle between normal and outgoing direction
    Random height from surface on the sphere is 1 - <0;1) = <0;1) cos(theta)
    float cosThetaUniform = 1.0f - rnd2
    Get more points as we get closer to the normal (increasing cosine)
    sqrt() - 0.01->0.1, 0.25->0.5, 0.5->0.71 */
    float rnd2 = rng.GetFloat();
    float cosTheta = std::sqrt(1.0f - rnd2); // sample height

    // Horizontal radius in height cosTheta, gotten from Pyhagoras:
    // r^2+cosTheta^2 = 1; r = sqrt(1-(1-rnd2))
    float r = std::sqrt(rnd2);
    return {r * std::cos(phi), r * std::sin(phi), cosTheta};
}

inline float getSpecularPdf(const Vec3f& inDir, const Vec3f& outDir, float phongExponent)
{
    float Refl_ExpNormalized = (phongExponent + 1) * 0.5f * INV_PI_F;
    Vec3f idealReflection = ReflectLocal(inDir);
    float cosTheta = std::max(0.0f, Dot(idealReflection, outDir));
    return Refl_ExpNormalized * std::pow(cosTheta, phongExponent);
}

inline float getDiffusePdf(const Vec3f& outDir)
{
    return outDir.z * INV_PI_F;
}

/**
     * Returns the probability density corresponding to sampleReflectedDirection,
     * i.e., what is the probability that calling sampleReflectedDirection would randomly choose the given outgoingDirection
     * Arguments:
     *  - incomingDirection = a normalized direction towards the previous (origin) point in the scene
     *  - outgoingDirection = the randomly sampled (normalized) outgoing direction
     */
inline float PdfCos(const Vec3f& inDir, const Vec3f& outDir)
{
    return outDir.z > 0.0f ? outDir.z * INV_PI_F : 0.0f;
}

inline float PdfUniform(const Vec3f& inDir, const Vec3f& outDir)
{
    return outDir.z > 0.0f ? 0.5f * INV_PI_F : 0.0f;
}
