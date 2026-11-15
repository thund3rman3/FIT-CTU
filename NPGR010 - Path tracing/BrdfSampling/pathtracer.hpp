#pragma once

#include <vector>
#include <cmath>
#include <omp.h>
#include <cassert>
#include <ostream>

#include "renderer.hpp"
#include "rng.hpp"
#include "framebuffer.hpp"
#include "materials.hpp"
#include "lights.hpp"
#include "scene.hpp"

class PathTracer : public AbstractRenderer {
public:
    explicit PathTracer(const Scene &aScene, int aSeed = 1234)
        : AbstractRenderer(aScene), mRandomGenerator(aSeed) {
    }

    Vec3f EmittedRadiance(const Ray &ray, const std::optional<Intersection> &hit) {
        if (!hit.has_value()) {
            const auto *bg = mScene.GetBackground();
            return bg ? bg->Evaluate(ray.direction) : Vec3f(0);
        }
        if (hit->lightID >= 0)
            return mScene.GetLightPtr(hit->lightID)->Evaluate(-ray.direction);
        return Vec3f(0.0f);
    }

    Vec3f GetDirectLight(const Ray &ray) {
        const auto intersection = mScene.FindClosestIntersection(ray);

        if (!intersection.has_value() || intersection->lightID >= 0)
            return EmittedRadiance(ray, intersection);

        const Vec3f surfacePoint = ray.origin + ray.direction * intersection->distance;
        CoordinateFrame frame;
        frame.SetFromZ(intersection->normal);
        const Vec3f incomingDirection = frame.ToLocal(-ray.direction);
        const Material &mat = mScene.GetMaterial(intersection->materialID);

        auto [localOutDir, brdf, pdf] = mat.SampleImportanceBrdf(incomingDirection, mRandomGenerator);
        float cosTheta = localOutDir.z;

        if (cosTheta <= 0.0f || brdf.Max() <= 0.0f || pdf <= 0.0f) {
            return Vec3f(0.0f);
        }
        
        // Note! To prevent intersecting the same object we are already on, we need to offset the ray by EPSILON_RAY
        Ray rayToLight(surfacePoint, frame.ToWorld(localOutDir), EPSILON_RAY);
        const auto intersectionRefl = mScene.FindClosestIntersection(rayToLight);
        Vec3f intensity = EmittedRadiance(rayToLight, intersectionRefl);

        return intensity * brdf * cosTheta / pdf;;
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
