#pragma once

#include <vector>
#include <cmath>
#include <utility>
#include <tuple>
#include <stdexcept>
#include "math.hpp"
#include "rng.hpp"

class AbstractLight {
public:
    /**
     * Randomly chooses a point on the light source
     * Arguments:
     *  - origin = our current position in the scene
     *  - rng = random generator
     * Returns:
     *  - a randomly sampled point on the light source
     *  - the illumination intensity corresponding to the sampled direction
     *  - the probability density (PDF) of choosing this point
     */
    virtual std::tuple<Vec3f, Vec3f, float> SamplePointOnLight(const Vec3f &origin, Rng &rng) const = 0;

    /**
     * Returns the probability density corresponding to samplePointOnLight,
     * i.e., what is the probability that calling samplePointOnLight would randomly choose the given lightPoint
     * Arguments:
     *  - origin = our current position in the scene
     *  - lightPoint = the randomly sampled point on the light source
     */
    virtual float PDF(const Vec3f &origin, const Vec3f &lightPoint) const = 0;

    /**
     * Returns the illumination intensity in the given direction
     * Arguments:
     *  - direction = direction from the light source
     */
    virtual Vec3f Evaluate(const Vec3f &dirFromLight) const = 0;

    virtual ~AbstractLight() = default;
};

//////////////////////////////////////////////////////////////////////////
class AreaLight : public AbstractLight {
public:
    AreaLight(const Vec3f &A, const Vec3f &B, const Vec3f &C) {
        p0 = A;
        e1 = B - A;
        e2 = C - A;

        Vec3f normal = Cross(e1, e2);
        float len = normal.Length();
        mInvArea = 2.f / len;
        mFrame.SetFromZ(normal);
    }

    std::tuple<Vec3f, Vec3f, float> SamplePointOnLight(const Vec3f &origin, Rng &rng) const override {
        float rnd1 = rng.GetFloat();
        float rnd2 = rng.GetFloat();
        float b1 = rnd1 + rnd2 > 1.0f ? 1.0f - rnd1 : rnd1;
        float b2 = rnd1 + rnd2 > 1.0f ? 1.0f - rnd2 : rnd2;
        Vec3f pointOnLight = p0 + e1 * b1 + e2 * b2;

        float pdf = PDF(origin, pointOnLight);
        Vec3f dirFromLight = origin - pointOnLight;
        dirFromLight = Normalize(dirFromLight);

        return {pointOnLight, Evaluate(dirFromLight), pdf};
    }

    float PDF(const Vec3f &origin, const Vec3f &lightPoint) const override {
        Vec3f dirToLight = lightPoint - origin;
        float dstSquared = dirToLight.LenSqr();
        dirToLight = Normalize(dirToLight);
        float cosThetaLight = Dot(mFrame.Normal(), -dirToLight);

        if (cosThetaLight <= 0)
            return 0.0f;

        return dstSquared * mInvArea / cosThetaLight;
    }

    Vec3f Evaluate(const Vec3f &dirFromLight) const override {
        float cosTheta = Dot(mFrame.Normal(), dirFromLight);
        return cosTheta > 0.0f ? mRadiance : Vec3f(0);
    }

public:
    CoordinateFrame mFrame;
    Vec3f p0, e1, e2;
    Vec3f mRadiance;
    float mInvArea; // pdf
};

//////////////////////////////////////////////////////////////////////////
class PointLight : public AbstractLight {
public:
    explicit PointLight(const Vec3f &aPosition) {
        mPosition = aPosition;
    }

    std::tuple<Vec3f, Vec3f, float> SamplePointOnLight(const Vec3f &origin, Rng &rng) const override {
        Vec3f outgoingDirection = mPosition - origin;
        float distanceSquared = outgoingDirection.LenSqr();

        return {mPosition, mIntensity / distanceSquared, 1.0f};
    }

    float PDF(const Vec3f &origin, const Vec3f &lightPoint) const override {
        return 1.0f;
    }

    Vec3f Evaluate(const Vec3f &dirFromLight) const override {
        return Vec3f(0);
    }

public:
    Vec3f mPosition;
    Vec3f mIntensity;
};


//////////////////////////////////////////////////////////////////////////
class BackgroundLight : public AbstractLight {
public:
    BackgroundLight() {
        mBackgroundColor = Vec3f(135, 206, 250) / Vec3f(255.f);
        mRadius = 100.f; // a radius big enough to cover the whole scene
    }

    std::tuple<Vec3f, Vec3f, float> SamplePointOnLight(const Vec3f &origin, Rng &rng) const override {
        float rnd1 = rng.GetFloat();
        float cosTheta = 1.0f - 2.0f * rnd1; // <-1;1) sample height
        // Horizontal radius in height cosTheta, gotten from Pyhagoras:
        // r^2+cos^2Theta = 1; r = sqrt(1-(1-2*rnd1)^2); r = sqrt(4*(rnd1-rnd1^2))
        float r = std::sqrt(std::max(0.0f, 4 * (rnd1 - rnd1 * rnd1)));
        float phi = 2.0f * PI_F * rng.GetFloat(); // Azimuthal angle <0;2PI)

        Vec3f dir(r * std::cos(phi), r * std::sin(phi), cosTheta);
        Vec3f lightPoint = origin + dir * mRadius;

        return {lightPoint, mBackgroundColor, PDF(origin, lightPoint)};
    }

    float PDF(const Vec3f &origin, const Vec3f &lightPoint) const override {
        return 1 / (4.0f * PI_F);
    }

    Vec3f Evaluate(const Vec3f &dirFromLight) const override {
        return mBackgroundColor;
    }

public:
    Vec3f mBackgroundColor;
    // we model the background light as a huge sphere around the whole scene, with a given radius
    float mRadius;
};
