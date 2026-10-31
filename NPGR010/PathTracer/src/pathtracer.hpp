#pragma once

#include <vector>
#include <cmath>
#include <omp.h>
#include <cassert>
#include <ostream>

#include "renderer.hpp"
#include "rng.hpp"

class PathTracer : public AbstractRenderer {
public:
    explicit PathTracer(const Scene &aScene, int aSeed = 1234)
        : AbstractRenderer(aScene), mRandomGenerator(aSeed) {
    }

    Vec3f SampleLight(const AbstractLight*& light, const CoordinateFrame& frame, const Vec3f& surfacePoint, const Material& mat, const Vec3f& incomingDirection) {
        auto [lightPoint, intensity, pdf] = light->SamplePointOnLight(surfacePoint, mRandomGenerator);
        Vec3f dirToLight = lightPoint - surfacePoint;
        float lightDistance = sqrt(dirToLight.LenSqr());
        Vec3f outgoingDirection = dirToLight / lightDistance;
        Vec3f localOutDir = frame.ToLocal(outgoingDirection);
        float cosTheta = localOutDir.z; //Dot(frame.Normal(), outgoingDirection);

        if (cosTheta <= 0.0f || intensity.Max() <= 0.0f || pdf <= 0.0f) {
            return Vec3f(0.0f);
        }

        // Note! To prevent intersecting the same object we are already on, we need to offset the ray by EPSILON_RAY
        Ray rayToLight(surfacePoint, outgoingDirection, EPSILON_RAY);
        // Testing if the direction towards the light source is not occluded
        if (mScene.FindAnyIntersection(rayToLight, lightDistance)) {
            return Vec3f(0.0f);
        }

        Vec3f brdf = mat.EvaluateBRDF(incomingDirection, localOutDir);
        return intensity * brdf * cosTheta / pdf;
    }

    Vec3f GetDirectLight(const Ray &ray) {
        const auto intersection = mScene.FindClosestIntersection(ray);

        Vec3f LoDirect = Vec3f(0.0f);
        if (!intersection.has_value()) {
            const auto* bg = mScene.GetBackground();
            return bg ? bg->Evaluate(ray.direction) : LoDirect;
        }

        if (intersection->lightID >= 0)
            return mScene.GetLightPtr(intersection->lightID)->Evaluate(-ray.direction);

        const Vec3f surfacePoint = ray.origin + ray.direction * intersection->distance;
        CoordinateFrame frame;
        frame.SetFromZ(intersection->normal);
        const Vec3f incomingDirection = frame.ToLocal(-ray.direction);
        const Material &mat = mScene.GetMaterial(intersection->materialID);

        // Connect from the current surface point to every light source in the scene:
        for (int i = 0; i < mScene.GetLightCount(); i++) {
            if (const AbstractLight *light = mScene.GetLightPtr(i))
                LoDirect += SampleLight(light, frame, surfacePoint, mat, incomingDirection);
        }

        return LoDirect;
    }

    void RunIteration() override {
        const int resolutionX = static_cast<int>(mScene.mCamera.mResolution.x);
        const int resolutionY = static_cast<int>(mScene.mCamera.mResolution.y);

        for (int pixelID = 0; pixelID < resolutionX * resolutionY; pixelID++) {
            // Current pixel coordinates (as integers):
            const int x = pixelID % resolutionX;
            const int y = pixelID / resolutionX;

            // Current pixel coordinates (as floating point numbers, randomly positioned inside a pixel square):
            // E.g., for x = 5, y = 12, we can have sample coordinates from x = 5.00 to 5.99.., and y = 12.00 to 12.99..
            const Vec2f sample = Vec2f(static_cast<float>(x), static_cast<float>(y)) + mRandomGenerator.GetVec2f();

            // Generating a ray with an origin in the camera with a direction corresponding to the pixel coordinates:
            Ray ray = mScene.mCamera.GenerateRay(sample);
            Vec3f LoDirect = GetDirectLight(ray);
            mFramebuffer.AddColor(sample, LoDirect);
        }

        mIterations++;
    }

public:
    Rng mRandomGenerator;
};
