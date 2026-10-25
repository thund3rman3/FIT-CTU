#pragma once

#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <filesystem>
#include <stdexcept>

#include "utils.hpp"

class Framebuffer {
public:
    Framebuffer() {
    }

    //////////////////////////////////////////////////////////////////////////
    // Accumulation
    void AddColor(const Vec2f &aSample, const Vec3f &aColor) {
        if (aSample.x < 0 || aSample.x >= mResolution.x)
            return;

        if (aSample.y < 0 || aSample.y >= mResolution.y)
            return;

        int x = static_cast<int>(aSample.x);
        int y = static_cast<int>(aSample.y);

        mPixels[x + y * mResX] = mPixels[x + y * mResX] + aColor;
    }

    //////////////////////////////////////////////////////////////////////////
    // Methods for framebuffer operations
    void Setup(const Vec2f &aResolution) {
        mResolution = aResolution;
        mResX = static_cast<int>(aResolution.x);
        mResY = static_cast<int>(aResolution.y);
        mPixels.resize(mResX * mResY);
        Clear();
    }

    void Clear() {
        // memset(&mColor[0], 0, sizeof(Vec3f) * mColor.size()); // Gives warnings
        mPixels.assign(mPixels.size(), 0);
    }

    void Add(const Framebuffer &aOther) {
        for (size_t i = 0; i < mPixels.size(); i++)
            mPixels[i] = mPixels[i] + aOther.mPixels[i];
    }

    void Scale(float aScale) {
        for (size_t i = 0; i < mPixels.size(); i++)
            mPixels[i] = mPixels[i] * Vec3f(aScale);
    }

    //////////////////////////////////////////////////////////////////////////
    // Statistics
    float TotalLuminance() const {
        float lum = 0;

        for (int y = 0; y < mResY; y++) {
            for (int x = 0; x < mResX; x++) {
                lum += Luminance(mPixels[x + y * mResX]);
            }
        }

        return lum;
    }

    // Saving

    void Save(const std::filesystem::path& outputName, const std::string& extension, float aGamma = 2.2f) const {
        if (extension == "bmp") {
            SaveBMP(outputName, 2.2f /*gamma*/);
        } else if (extension == "hdr") {
            SaveHDR(outputName);
        } else if (extension == "pfm") {
            SavePFM(outputName);
        } else if (extension == "ppm") {
            SavePPM(outputName, 2.2f);
        } else {
            printf("Used unknown extension %s\n", extension.c_str());
        }

        printf("done\n");
    }

    //////////////////////////////////////////////////////////////////////////
    void SavePPM(const std::filesystem::path& aFilename, float aGamma = 2.2f) const {
        const float invGamma = 1.f / aGamma;

        std::ofstream ppm(aFilename);
        if (!ppm)
            throw std::runtime_error("Could not open file for writing");

        ppm << "P3\n" << mResX << " " << mResY << "\n255" << std::endl;

        for (int y = 0; y < mResY; y++) {
            for (int x = 0; x < mResX; x++) {
                const Vec3f *ptr = &mPixels[x + y * mResX];
                int r = static_cast<int>(std::pow(ptr->x, invGamma) * 255.f);
                int g = static_cast<int>(std::pow(ptr->y, invGamma) * 255.f);
                int b = static_cast<int>(std::pow(ptr->z, invGamma) * 255.f);

                ppm << std::min(255, std::max(0, r)) << " " << std::min(255, std::max(0, g)) << " " << std::min(
                    255, std::max(0, b)) << " ";
            }

            ppm << std::endl;
        }
        ppm.close();
    }

    void SavePFM(const std::filesystem::path& aFilename) const {
        std::ofstream pfm(aFilename, std::ios::binary);

        if (!pfm)
            throw std::runtime_error("Could not open file for writing");

        pfm << "PF\n" << mResX << " " << mResY << "\n-1.0" << std::endl;

        for (int y = mResY - 1; y >= 0; --y) {
            for (int x = 0; x < mResX; x++) {
                pfm.write(reinterpret_cast<const char *>(&mPixels[x + y * mResX]), sizeof(Vec3f));
            }
        }
        pfm.close();
    }

    //////////////////////////////////////////////////////////////////////////
    // Saving BMP
    struct BmpHeader {
        uint mFileSize;        // Size of file in bytes
        uint mReserved01;      // 2x 2 reserved bytes
        uint mDataOffset;      // Offset in bytes where data can be found (54)

        uint mHeaderSize;      // 40B
        int mWidth;            // Width in pixels
        int mHeight;           // Height in pixels

        short mColorPlates;    // Must be 1
        short mBitsPerPixel;   // We use 24bpp
        uint mCompression;     // We use BI_RGB ~ 0, uncompressed
        uint mImageSize;       // mWidth x mHeight x 3B
        uint mHorizRes;        // Pixels per meter (75dpi ~ 2953ppm)
        uint mVertRes;         // Pixels per meter (75dpi ~ 2953ppm)
        uint mPaletteColors;   // Not using palette - 0
        uint mImportantColors; // 0 - all are important

        BmpHeader(int resX, int resY)
            : mFileSize(static_cast<uint>(sizeof(BmpHeader) + 2) + (resX * resY * 3)),
              mReserved01(0),
              mDataOffset(static_cast<uint>(sizeof(BmpHeader) + 2)),
              mHeaderSize(40),
              mWidth(resX),
              mHeight(resY),
              mColorPlates(1),
              mBitsPerPixel(24),
              mCompression(0),
              mImageSize(resX * resY * 3),
              mHorizRes(2953),
              mVertRes(2953),
              mPaletteColors(0),
              mImportantColors(0)
        {
        }
    };

    void SaveBMP(const std::filesystem::path& aFilename, float aGamma = 2.2f) const {
        std::ofstream bmp(aFilename, std::ios::binary);
        if (!bmp)
            throw std::runtime_error("Could not open file for writing");

        BmpHeader header(mResX, mResY);
        bmp.write("BM", 2);
        bmp.write(reinterpret_cast<char *>(&header), sizeof(header));

        const float invGamma = 1.f / aGamma;
        for (int y = 0; y < mResY; y++) {
            for (int x = 0; x < mResX; x++) {
                // bmp is stored from bottom up
                const Vec3f &rgbF = mPixels[x + (mResY - y - 1) * mResX];
                typedef unsigned char byte;
                float gammaBgr[3];
                gammaBgr[0] = std::pow(rgbF.z, invGamma) * 255.f;
                gammaBgr[1] = std::pow(rgbF.y, invGamma) * 255.f;
                gammaBgr[2] = std::pow(rgbF.x, invGamma) * 255.f;

                byte bgrB[3];
                bgrB[0] = static_cast<byte>(std::min(255.f, std::max(0.f, gammaBgr[0])));
                bgrB[1] = static_cast<byte>(std::min(255.f, std::max(0.f, gammaBgr[1])));
                bgrB[2] = static_cast<byte>(std::min(255.f, std::max(0.f, gammaBgr[2])));

                bmp.write(reinterpret_cast<char *>(&bgrB), sizeof(bgrB));
            }
        }
        bmp.close();
    }

    //////////////////////////////////////////////////////////////////////////
    // Saving HDR
    void SaveHDR(const std::filesystem::path& aFilename) const {
        std::ofstream hdr(aFilename, std::ios::binary);
        if (!hdr)
            throw std::runtime_error("Could not open file for writing");

        hdr << "#?RADIANCE" << '\n';
        hdr << "# PG3Render" << '\n';
        hdr << "FORMAT=32-bit_rle_rgbe" << '\n' << '\n';
        hdr << "-Y " << mResY << " +X " << mResX << '\n';

        for (int y = 0; y < mResY; y++) {
            for (int x = 0; x < mResX; x++) {
                typedef unsigned char byte;
                byte rgbe[4] = {0, 0, 0, 0};

                const Vec3f &rgbF = mPixels[x + y * mResX];
                float v = std::max(rgbF.x, std::max(rgbF.y, rgbF.z));

                if (v >= 1e-32f) {
                    int e;
                    v = static_cast<float>(frexp(v, &e) * 256.f / v);
                    rgbe[0] = static_cast<byte>(rgbF.x * v);
                    rgbe[1] = static_cast<byte>(rgbF.y * v);
                    rgbe[2] = static_cast<byte>(rgbF.z * v);
                    rgbe[3] = static_cast<byte>(e + 128);
                }

                hdr.write(reinterpret_cast<char *>(&rgbe[0]), 4);
            }
        }
        hdr.close();
    }

private:
    std::vector<Vec3f> mPixels;
    Vec2f mResolution;
    int mResX{};
    int mResY{};
};
