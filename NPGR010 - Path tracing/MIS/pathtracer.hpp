#pragma once

#include <cassert>
#include <cmath>
#include <omp.h>
#include <ostream>
#include <vector>
// #include <print>

#include "framebuffer.hpp"
#include "lights.hpp"
#include "materials.hpp"
#include "renderer.hpp"
#include "rng.hpp"
#include "scene.hpp"

class PathTracer : public AbstractRenderer {
public:
    explicit PathTracer(const Scene& aScene, int aSeed = 1234)
            : AbstractRenderer(aScene), mRandomGenerator(aSeed) {}

    Vec3f EmittedRadiance(const Ray& ray, const std::optional<Intersection>& hit) {
        if (!hit.has_value()) {
            const auto* bg = mScene.GetBackground();
            return bg ? bg->Evaluate(ray.direction) : Vec3f(0.0f);
        }
        if (hit->lightID >= 0) {
            return mScene.GetLightPtr(hit->lightID)->Evaluate(-ray.direction);
        }
        return Vec3f(0.0f);
    }

    Vec3f GetDirectIlluminationLight(const AbstractLight& light, const Vec3f& surfacePoint,
        const CoordinateFrame& frame, const Material& mat, const Vec3f& incomingDirection) {
        auto [lightPoint, lightIntensity, pdfB] = light.SamplePointOnLight(surfacePoint, mRandomGenerator);

        Vec3f dirToLight        = lightPoint - surfacePoint;
        float lightDistance     = sqrt(dirToLight.LenSqr());
        Vec3f outgoingDirection = dirToLight / lightDistance;
        Vec3f localOutDirB      = frame.ToLocal(outgoingDirection);
        float cosThetaB         = localOutDirB.z;

        if (cosThetaB <= 0.0f || lightIntensity.Max() <= 0.0f || pdfB <= 0.0f) {
            return Vec3f(0.0f);
        }

        Ray shadowRay(surfacePoint, outgoingDirection, EPSILON_RAY);
        if (mScene.FindAnyIntersection(shadowRay, lightDistance)) {
            return Vec3f(0.0f);
        }

        Vec3f brdf           = mat.EvaluateBRDF(incomingDirection, localOutDirB);
        float pdfAsIfFromA_B = light.IsPoint() ? 0.0f : mat.EvalPdf(incomingDirection, localOutDirB);
        float weightMIS_B    = pdfB / (pdfB + pdfAsIfFromA_B);

        return weightMIS_B * lightIntensity * brdf * cosThetaB / pdfB;
    }

    // brdf sampling
    Vec3f GetDirectIlluminationBRDF(
        const Vec3f& surfacePoint, const CoordinateFrame& frame, const Material& mat, const Vec3f& incomingDirection) {
        const auto [localOutDirA, brdfA, pdfA] = mat.SampleBRDF(incomingDirection, mRandomGenerator);

        float cosThetaA = localOutDirA.z;
        if (cosThetaA <= 0.0f || brdfA.Max() <= 0.0f || pdfA <= 0.0f) {
            return Vec3f(0.0f);
        }

        Vec3f worldOutDir = frame.ToWorld(localOutDirA);
        Ray shadowRay(surfacePoint, worldOutDir, EPSILON_RAY);

        const auto intersectionShadowRay = mScene.FindClosestIntersection(shadowRay);
        Vec3f emittedRadiance            = EmittedRadiance(shadowRay, intersectionShadowRay);

        if (emittedRadiance.Max() <= 0.0f) {
            return Vec3f(0.0f);
        }

        float pdfAsIfFromB_A(0.0f);

        if (intersectionShadowRay.has_value() && intersectionShadowRay->lightID >= 0) {
            const Vec3f lightPoint = shadowRay.origin + shadowRay.direction * intersectionShadowRay->distance;
            int hitLight           = intersectionShadowRay->lightID;
            pdfAsIfFromB_A         = mScene.GetLightPtr(hitLight)->PDF(surfacePoint, lightPoint);
        } else if (const auto* bg = mScene.GetBackground()) {
            pdfAsIfFromB_A = bg->PDF(surfacePoint, surfacePoint);
        }
        float weightMIS_A = pdfA / (pdfA + pdfAsIfFromB_A);

        return weightMIS_A * emittedRadiance * brdfA * cosThetaA / pdfA;
    }

    Vec3f GetDirectIlluminationMIS(const Ray& ray, const Intersection& hit) {
        const Vec3f surfacePoint = ray.origin + ray.direction * hit.distance;
        CoordinateFrame frame;
        frame.SetFromZ(hit.normal);
        const Vec3f incomingDirection = frame.ToLocal(-ray.direction);
        const Material& mat           = mScene.GetMaterial(hit.materialID);

        // A: BRDF Sampling
        Vec3f estimatedRadiance = GetDirectIlluminationBRDF(surfacePoint, frame, mat, incomingDirection);

        // B: Light Sampling
        for (int i = 0; i < mScene.GetLightCount(); i++) {
            const AbstractLight* light = mScene.GetLightPtr(i);
            if (!light) {
                continue;
            }
            estimatedRadiance += GetDirectIlluminationLight(*light, surfacePoint, frame, mat, incomingDirection);
        }
        return estimatedRadiance;
    }

    void RunIteration() override {
        const int resolutionX = static_cast<int>(mScene.mCamera.mResolution.x);
        const int resolutionY = static_cast<int>(mScene.mCamera.mResolution.y);
        Vec3f LoDirect;

        for (int pixelID = 0; pixelID < resolutionX * resolutionY; pixelID++) {
            const int x = pixelID % resolutionX;
            const int y = pixelID / resolutionX;

            const Vec2f sample      = Vec2f(static_cast<float>(x), static_cast<float>(y)) + mRandomGenerator.GetVec2f();
            Ray ray                 = mScene.mCamera.GenerateRay(sample);
            const auto intersection = mScene.FindClosestIntersection(ray);

            const Vec3f radiance = EmittedRadiance(ray, intersection);
            if (radiance.Max() > 0.0f) {
                LoDirect = radiance;
            } else {
                LoDirect = GetDirectIlluminationMIS(ray, intersection.value());
            }

            mFramebuffer.AddColor(sample, LoDirect);
        }

        mIterations++;
    }

public:
    Rng mRandomGenerator;
};
