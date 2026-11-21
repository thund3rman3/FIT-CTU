#pragma once

#include <omp.h>

#include <cassert>
#include <cmath>
#include <ostream>
#include <vector>

#include "framebuffer.hpp"
#include "lights.hpp"
#include "materials.hpp"
#include "renderer.hpp"
#include "rng.hpp"
#include "scene.hpp"

class PathTracer : public AbstractRenderer {
 public:
  explicit PathTracer(const Scene &aScene, int aSeed = 1234)
      : AbstractRenderer(aScene), mRandomGenerator(aSeed) {}

  Vec3f EmittedRadiance(const Ray &ray,
                        const std::optional<Intersection> &hit) {
    if (!hit.has_value()) {
      const auto *bg = mScene.GetBackground();
      return bg ? bg->Evaluate(ray.direction) : Vec3f(0.0f);
    }
    if (hit->lightID >= 0)
      return mScene.GetLightPtr(hit->lightID)->Evaluate(-ray.direction);
    return Vec3f(0.0f);
  }

  // Light sampling
  Vec3f EvalLight(const AbstractLight *&light, const CoordinateFrame &frame,
                  const Vec3f &surfacePoint, const Material &mat,
                  const Vec3f &incomingDirection) {
    auto [lightPoint, intensity, pdf] =
        light->SamplePointOnLight(surfacePoint, mRandomGenerator);
    Vec3f dirToLight = lightPoint - surfacePoint;
    float lightDistance = sqrt(dirToLight.LenSqr());
    Vec3f outgoingDirection = dirToLight / lightDistance;
    Vec3f localOutDir = frame.ToLocal(outgoingDirection);
    float cosTheta = localOutDir.z;

    if (cosTheta <= 0.0f || intensity.Max() <= 0.0f || pdf <= 0.0f) {
      return Vec3f(0.0f);
    }

    Ray shadowRay(surfacePoint, outgoingDirection, EPSILON_RAY);
    if (mScene.FindAnyIntersection(shadowRay, lightDistance)) {
      return Vec3f(0.0f);
    }

    Vec3f brdf = mat.EvaluateBRDF(incomingDirection, localOutDir);
    return intensity * brdf * cosTheta / pdf;
  }

  Vec3f GetDirectIlluminationLight(const Ray &ray, const Intersection &hit) {
    Vec3f LoDirect(0.0f);

    const Vec3f surfacePoint = ray.origin + ray.direction * hit.distance;
    CoordinateFrame frame;
    frame.SetFromZ(hit.normal);
    const Vec3f incomingDirection = frame.ToLocal(-ray.direction);
    const Material &mat = mScene.GetMaterial(hit.materialID);

    // Connect from the current surface point to every light source in the
    // scene:
    for (int i = 0; i < mScene.GetLightCount(); i++) {
      if (const AbstractLight *light = mScene.GetLightPtr(i))
        LoDirect +=
            EvalLight(light, frame, surfacePoint, mat, incomingDirection);
    }

    return LoDirect;
  }

  // brdf sampling

  Vec3f GetDirectIlluminationBRDF(const Ray &ray, const Intersection &hit) {
    const Vec3f surfacePoint = ray.origin + ray.direction * hit.distance;
    CoordinateFrame frame;
    frame.SetFromZ(hit.normal);
    const Vec3f incomingDirection = frame.ToLocal(-ray.direction);
    const Material &mat = mScene.GetMaterial(hit.materialID);

    auto [localOutDir, brdf, pdf] =
        mat.SampleBRDF(incomingDirection, mRandomGenerator);
    float cosTheta = localOutDir.z;

    if (cosTheta <= 0.0f || brdf.Max() <= 0.0f || pdf <= 0.0f) {
      return Vec3f(0.0f);
    }

    Ray shadowRay(surfacePoint, frame.ToWorld(localOutDir), EPSILON_RAY);
    const auto intersectionShadowRay =
        mScene.FindClosestIntersection(shadowRay);
    Vec3f intensity = EmittedRadiance(shadowRay, intersectionShadowRay);

    return intensity * brdf * cosTheta / pdf;
  }

  void RunIteration() override {
    const int resolutionX = static_cast<int>(mScene.mCamera.mResolution.x);
    const int resolutionY = static_cast<int>(mScene.mCamera.mResolution.y);
    Vec3f LoDirect;

    for (int pixelID = 0; pixelID < resolutionX * resolutionY; pixelID++) {
      const int x = pixelID % resolutionX;
      const int y = pixelID / resolutionX;

      const Vec2f sample = Vec2f(static_cast<float>(x), static_cast<float>(y)) +
                           mRandomGenerator.GetVec2f();

      Ray ray = mScene.mCamera.GenerateRay(sample);
      const auto intersection = mScene.FindClosestIntersection(ray);

      const Vec3f radiance = EmittedRadiance(ray, intersection);
      if (radiance.Max() > 0.0f)
        LoDirect = radiance;
      else {
        LoDirect = GetDirectIlluminationBRDF(ray, intersection.value());
        // LoDirect = GetDirectIlluminationLight(ray, intersection.value());
      }

      mFramebuffer.AddColor(sample, LoDirect);
    }

    mIterations++;
  }

 public:
  Rng mRandomGenerator;
};
