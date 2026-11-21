#pragma once

#include <stdexcept>
#include <tuple>
#include <utility>

#include "math.hpp"
#include "rng.hpp"
#include "utils.hpp"

class Material {
public:
    Material() {
        Reset();
    }

    void Reset() {
        mDiffuseReflectance = Vec3f(0);
        mPhongReflectance   = Vec3f(0);
        mPhongExponent      = 1.f;
    }

    void SetReflectanceProbs() {
        mProbDiffuse        = mDiffuseReflectance.Max();
        mProbSpecular       = mPhongReflectance.Max();
        float normalization = 1.f / (mProbDiffuse + mProbSpecular);

        mProbDiffuse *= normalization;
        mProbSpecular *= normalization;
    }

    std::tuple<Vec3f, Vec3f, float> SampleBRDF(const Vec3f& incomingDirection, Rng& rng) const {
        Vec3f reflectedDir;
        if (rng.GetFloat() < mProbDiffuse) {
            reflectedDir = SampleDiffuse(rng);
        } else {
            reflectedDir = SampleSpecular(incomingDirection, rng);
        }

        float pdf = EvalPdf(incomingDirection, reflectedDir);
        return {reflectedDir, EvaluateBRDF(incomingDirection, reflectedDir), pdf};
    }

    float EvalPdf(const Vec3f& inDir, const Vec3f& outDir) const {
        if (outDir.z <= 0.0f) {
            return 0.0f;
        }
        return mProbDiffuse * getDiffusePdf(outDir) + mProbSpecular * getSpecularPdf(inDir, outDir, mPhongExponent);
    }

    Vec3f SampleDiffuse(Rng& rng) const {
        // Hemispherical sampling proportional to cos(theta)
        return SampleHemisphereCos(rng);
    }

    Vec3f SampleSpecular(const Vec3f& incomingDirection, Rng& rng) const {
        CoordinateFrame lobeFrame;
        lobeFrame.SetFromZ(ReflectLocal(incomingDirection));

        float phi      = 2.0f * PI_F * rng.GetFloat();
        float rnd2     = rng.GetFloat();
        float exp      = 1.0f / (mPhongExponent + 1.0f);
        float cosTheta = std::pow(rnd2, exp); // sample height
        float r        = std::sqrt(1.0f - std::pow(rnd2, 2.0f * exp));
        Vec3f dirInLobeFrame(r * std::cos(phi), r * std::sin(phi), cosTheta);
        return lobeFrame.ToWorld(dirInLobeFrame); // convert from Lobe frame to Local/Shading frame
    }

    /**
     * Returns the intensity corresponding to the reflected light according to
     * this material's BRDF Arguments:
     *  - incomingDirection = a normalized direction towards the previous (origin)
     * point in the scene
     *  - outgoingDirection = a normalized outgoing reflected direction
     */
    Vec3f EvaluateBRDF(const Vec3f& incomingDirection, const Vec3f& outgoingDirection) const {
        if (incomingDirection.z <= 0 || outgoingDirection.z <= 0) {
            return Vec3f(0);
        }

        Vec3f diffuseComponent = mDiffuseReflectance * INV_PI_F;

        if (mPhongReflectance.Max() <= 0.0f) {
            return diffuseComponent;
        }

        Vec3f idealReflection = ReflectLocal(incomingDirection);

        float cosTheta = Dot(idealReflection, outgoingDirection);
        cosTheta       = std::max(0.0f, cosTheta);

        Vec3f Refl_ExpNormalized = mPhongReflectance * (mPhongExponent + 2.0f) / (2.0f * PI_F);
        Vec3f glossyComponent    = Refl_ExpNormalized * std::pow(cosTheta, mPhongExponent);

        return diffuseComponent + glossyComponent;
    }

    std::tuple<Vec3f, Vec3f, float> SampleReflectedDirectionCosine(const Vec3f& incomingDirection, Rng& rng) const {
        Vec3f outDir    = SampleHemisphereCos(rng);
        Vec3f intensity = EvaluateBRDF(incomingDirection, outDir);
        float pdf       = PdfCos(incomingDirection, outDir);
        return {outDir, intensity, pdf};
    }

    std::tuple<Vec3f, Vec3f, float> SampleReflectedDirectionUniform(const Vec3f& incomingDirection, Rng& rng) const {
        Vec3f outDir    = SampleHemisphereUniform(rng);
        Vec3f intensity = EvaluateBRDF(incomingDirection, outDir);
        float pdf       = PdfUniform(incomingDirection, outDir);
        return {outDir, intensity, pdf};
    }

public:
    Vec3f mDiffuseReflectance;
    Vec3f mPhongReflectance;
    float mPhongExponent{};
    // probability of choosing the diffuse component
    float mProbDiffuse;
    // probability of choosing the specular component
    float mProbSpecular;
};
