#pragma once

#include <utility>
#include <tuple>
#include <stdexcept>
#include "math.hpp"
#include "rng.hpp"

class Material {
public:
    Material() {
        Reset();
    }

    void Reset() {
        mDiffuseReflectance = Vec3f(0);
        mPhongReflectance = Vec3f(0);
        mPhongExponent = 1.f;
    }

    /**
     * Randomly chooses an outgoing direction reflected from the material surface
     * Arguments:
     *  - incomingDirection = a normalized direction towards the previous (origin) point in the scene
     *  - rng = random generator
     * Returns:
     *  - a randomly sampled reflected outgoing direction
     *  - the intensity corresponding to the reflected light
     *  - the probability density (PDF) of choosing this direction
     */
    std::tuple<Vec3f, Vec3f, float> SampleReflectedDirectionCosine(const Vec3f &incomingDirection, Rng &rng) const {
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
        Vec3f outDir(r * std::cos(phi), r * std::sin(phi), cosTheta);

        Vec3f intensity = EvaluateBRDF(incomingDirection, outDir);
        float pdf = PdfCos(incomingDirection, outDir);
        return {outDir, intensity, pdf};
    }

    std::tuple<Vec3f, Vec3f, float> SampleReflectedDirectionUniform(const Vec3f &incomingDirection, Rng &rng) const {
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
        Vec3f outDir(r * std::cos(phi), r * std::sin(phi), cosTheta);

        Vec3f intensity = EvaluateBRDF(incomingDirection, outDir);
        float pdf = PdfUniform(incomingDirection, outDir);
        return {outDir, intensity, pdf};
    }

    /**
     * Returns the probability density corresponding to sampleReflectedDirection,
     * i.e., what is the probability that calling sampleReflectedDirection would randomly choose the given outgoingDirection
     * Arguments:
     *  - incomingDirection = a normalized direction towards the previous (origin) point in the scene
     *  - outgoingDirection = the randomly sampled (normalized) outgoing direction
     */
    float PdfCos(const Vec3f &incomingDirection, const Vec3f &outgoingDirection) const {
        return outgoingDirection.z > 0.0f ? outgoingDirection.z * INV_PI_F : 0.0f;
    }

    float PdfUniform(const Vec3f &incomingDirection, const Vec3f &outgoingDirection) const {
        return outgoingDirection.z > 0.0f ? 0.5f * INV_PI_F : 0.0f;
    }

    /**
     * Returns the intensity corresponding to the reflected light according to this material's BRDF
     * Arguments:
     *  - incomingDirection = a normalized direction towards the previous (origin) point in the scene
     *  - outgoingDirection = a normalized outgoing reflected direction
     */
    Vec3f EvaluateBRDF(const Vec3f &incomingDirection, const Vec3f &outgoingDirection) const {
        if (incomingDirection.z <= 0 || outgoingDirection.z <= 0)
            return Vec3f(0);

        Vec3f diffuseComponent = mDiffuseReflectance * INV_PI_F;

        if (mPhongReflectance.Max() <= 0.0f)
            return diffuseComponent;

        Vec3f idealReflection(-incomingDirection.x, -incomingDirection.y, incomingDirection.z);

        float cosAlpha = Dot(idealReflection, outgoingDirection);
        cosAlpha = std::max(0.0f, cosAlpha);

        Vec3f Refl_ExpNormalized = mPhongReflectance * (mPhongExponent + 2.0f) / (2.0f * PI_F);
        Vec3f glossyComponent =  Refl_ExpNormalized * std::pow(cosAlpha, mPhongExponent);

        return diffuseComponent + glossyComponent;
    }

public:
    Vec3f mDiffuseReflectance;
    Vec3f mPhongReflectance;
    float mPhongExponent{};
};
