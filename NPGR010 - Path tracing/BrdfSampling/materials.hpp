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

    std::tuple<Vec3f, Vec3f, float> SampleImportanceBrdf(const Vec3f &incomingDirection, Rng &rng) const {
        float probDiffuse  = mDiffuseReflectance.Max();
        float probSpecular = mPhongReflectance.Max();
        float normalization = 1.f / (probDiffuse + probSpecular);

        // probability of choosing the diffuse component
        probDiffuse *= normalization;
        // probability of choosing the specular component
        probSpecular *= normalization;

        Vec3f generatedDir;
        if (rng.GetFloat() <= probDiffuse)
            generatedDir = SampleDiffuse(incomingDirection, rng);
        else
            generatedDir = SampleSpecular(incomingDirection, rng);

        float pdf = EvalPdf(incomingDirection, generatedDir, probDiffuse, probSpecular);
        return {generatedDir, EvaluateBRDF(incomingDirection, generatedDir), pdf};
    }

    Vec3f SampleDiffuse(const Vec3f &incomingDirection, Rng &rng) const {
        // Hemispherical sampling proportional to cos(theta)

        float phi = 2.0f * PI_F * rng.GetFloat();
        float rnd2 = rng.GetFloat();
        float cosTheta = std::sqrt(1.0f - rnd2); // sample height
        float r = std::sqrt(rnd2);
        return Vec3f(r * std::cos(phi), r * std::sin(phi), cosTheta);
    }

    Vec3f SampleSpecular(const Vec3f &incomingDirection, Rng &rng) const {
        const Vec3f idealReflection(-incomingDirection.x, -incomingDirection.y, incomingDirection.z);
        CoordinateFrame lobeFrame;
        lobeFrame.SetFromZ(idealReflection);

        float phi = 2.0f * PI_F * rng.GetFloat();
        float rnd2 = rng.GetFloat();
        float exp = 1.0f / (mPhongExponent + 1.0f);
        float cosTheta = std::pow(rnd2, exp); // sample height
        float r = std::sqrt(1.0f - std::pow(rnd2, 2.0f * exp));
        Vec3f dirInLobeFrame(r * std::cos(phi), r * std::sin(phi), cosTheta);
        return lobeFrame.ToWorld(dirInLobeFrame);
    }

    /**
     * Returns the probability density corresponding to sampleReflectedDirection,
     * i.e., what is the probability that calling sampleReflectedDirection would randomly choose the given outgoingDirection
     * Arguments:
     *  - incomingDirection = a normalized direction towards the previous (origin) point in the scene
     *  - outgoingDirection = the randomly sampled (normalized) outgoing direction
     */
    float getSpecularPdf(const Vec3f &incomingDirection, const Vec3f &outgoingDirection) const {
        float Refl_ExpNormalized = (mPhongExponent + 1) * 0.5f * INV_PI_F;
        Vec3f idealReflection(-incomingDirection.x, -incomingDirection.y, incomingDirection.z);
        float cosTheta = std::max(0.0f, Dot(idealReflection, outgoingDirection));
        return Refl_ExpNormalized * std::pow(cosTheta, mPhongExponent);
    }

    float getDiffusePdf(const Vec3f &outgoingDirection) const {
        return outgoingDirection.z * INV_PI_F;
    }

    float EvalPdf(const Vec3f& incomingDirection, const Vec3f& generatedDir, float probDiffuse, float probSpecular) const {
        if (generatedDir.z <= 0.0f)
            return 0.0f;
        return probDiffuse * getDiffusePdf(generatedDir) + probSpecular * getSpecularPdf(incomingDirection, generatedDir);
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

        float cosTheta = Dot(idealReflection, outgoingDirection);
        cosTheta = std::max(0.0f, cosTheta);

        Vec3f Refl_ExpNormalized = mPhongReflectance * (mPhongExponent + 2.0f) / (2.0f * PI_F);
        Vec3f glossyComponent =  Refl_ExpNormalized * std::pow(cosTheta, mPhongExponent);

        return diffuseComponent + glossyComponent;
    }

public:
    Vec3f mDiffuseReflectance;
    Vec3f mPhongReflectance;
    float mPhongExponent{};
};
