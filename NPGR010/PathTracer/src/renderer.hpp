#pragma once

#include <vector>
#include <cmath>
#include "scene.hpp"
#include "framebuffer.hpp"

class AbstractRenderer {
public:
    explicit AbstractRenderer(const Scene &aScene)
        : mScene(aScene) {
        mMinPathLength = 0;
        mMaxPathLength = 2;
        mIterations = 0;
        mFramebuffer.Setup(aScene.mCamera.mResolution);
    }

    virtual ~AbstractRenderer() {
    }

    virtual void RunIteration() = 0;

    void GetFramebuffer(Framebuffer &oFramebuffer) const {
        oFramebuffer = mFramebuffer;
        oFramebuffer.Scale(mIterations);
    }

    //! Whether this renderer was used at all
    bool WasUsed() const {
        return mIterations > 0;
    }

public:
    uint mMaxPathLength;
    uint mMinPathLength;

protected:
    int mIterations;
    Framebuffer mFramebuffer;
    const Scene &mScene;
};
