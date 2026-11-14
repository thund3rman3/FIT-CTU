/*!**************************************************************************
\file    TBRDF.cpp
\author  Jiri Filip, UTIA AS CR
\date    June 2013
\edited  Josef Jech, FIT CTU
\date    November 2026
\version 2.0 (C++23)

  The main file for:
   - the BRDF dataset loading from binary file (doubles) 
   - linear values interpolation for query incoming and outgoing directions
******************************************************************************/

#include "TBRDF.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <print>
#include <stdexcept>
#include <string>
#include <vector>

#include "../math/vec3.h"

BRDF::BRDF(const std::filesystem::path& filepath){
  Bd.resize(total_elements);
  loadBrdf(filepath);
}

void BRDF::loadBrdf(const std::filesystem::path& filepath)
{
    namespace fs = std::filesystem;

    if (!fs::exists(filepath)) {
        throw std::runtime_error(std::format("BRDF::loadBrdf: File does not exist: '{}'", filepath.string()));
    }

    if (!fs::is_regular_file(filepath)) {
        throw std::runtime_error(std::format("BRDF::loadBrdf: Path is not a regular file: '{}'", filepath.string()));
    }

    const std::uintmax_t expected_bytes = total_elements * sizeof(double);
    const std::uintmax_t actual_bytes = fs::file_size(filepath);

    if (actual_bytes != expected_bytes) {
        throw std::runtime_error(std::format(
            "BRDF::loadBrdf: File size mismatch for '{}'. Expected {} bytes ({} doubles), but got {} bytes.",
            filepath.string(), expected_bytes, total_elements, actual_bytes
        ));
    }

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error(std::format("BRDF::loadBrdf: Failed to open file: '{}'", filepath.string()));
    }

    // Fast bulk read of doubles from disk into temporary buffer
    std::vector<double> double_buffer(total_elements);
    if (!file.read(reinterpret_cast<char*>(double_buffer.data()), static_cast<std::streamsize>(expected_bytes))) {
        throw std::runtime_error(std::format("BRDF::loadBrdf: Failed to read data from file: '{}'", filepath.string()));
    }

    // Convert doubles to floats in member buffer (~972 KB instead of ~1.95 MB)
    if (Bd.size() != total_elements) {
        Bd.resize(total_elements);
    }

    for (std::size_t i = 0; i < total_elements; ++i) {
        Bd[i] = static_cast<float>(double_buffer[i]);
    }

    _filePath = filepath;
    _matName = filepath.stem().string();
    _isLoaded = true;

    std::println("Reading BRDF from file {} ...done ({} floats cached)", filepath.string(), total_elements);
}

vec3 BRDF::lookupBrdfVal(const vec3& illumination, const vec3& view) const
{
    if (!_isLoaded || Bd.empty()) {
        throw std::runtime_error("BRDF::lookupBrdfVal: BRDF data is not loaded. Call loadBrdf() before lookup.");
    }

    float theta_i, phi_i, theta_v, phi_v;
    std::tie(theta_i, phi_i) = getSphericalAngles(illumination);
    std::tie(theta_v, phi_v) = getSphericalAngles(view);

    constexpr float half_pi = PI * 0.5f;
    if (theta_i < 0.0f || theta_i > half_pi || theta_v < 0.0f || theta_v > half_pi) {
        return vec3(0.0f);
    }

    constexpr float r2d = 180.0f / PI;
    theta_i *= r2d;
    theta_v *= r2d;
    phi_i   *= r2d;
    phi_v   *= r2d;

    if (phi_i >= 360.0f) {
        phi_i = 0.0f;
    }
    if (phi_v >= 360.0f) {
        phi_v = 0.0f;
    }

    int iti[2];
    iti[0] = static_cast<int>(std::floor(theta_i * inv_step_t));
    iti[1] = iti[0] + 1;
    if (iti[0] > nti - 2) {
        iti[0] = nti - 2;
        iti[1] = nti - 1;
    }

    int itv[2];
    itv[0] = static_cast<int>(std::floor(theta_v * inv_step_t));
    itv[1] = itv[0] + 1;
    if (itv[0] > ntv - 2) {
        itv[0] = ntv - 2;
        itv[1] = ntv - 1;
    }

    int ipi[2];
    ipi[0] = static_cast<int>(std::floor(phi_i * inv_step_p));
    ipi[1] = ipi[0] + 1;

    int ipv[2];
    ipv[0] = static_cast<int>(std::floor(phi_v * inv_step_p));
    ipv[1] = ipv[0] + 1;

    const float wti1 = (theta_i - step_t * static_cast<float>(iti[0])) * inv_step_t;
    const float wti0 = 1.0f - wti1;

    const float wtv1 = (theta_v - step_t * static_cast<float>(itv[0])) * inv_step_t;
    const float wtv0 = 1.0f - wtv1;

    const float wpi1 = (phi_i - step_p * static_cast<float>(ipi[0])) * inv_step_p;
    const float wpi0 = 1.0f - wpi1;

    const float wpv1 = (phi_v - step_p * static_cast<float>(ipv[0])) * inv_step_p;
    const float wpv0 = 1.0f - wpv1;

    if (ipi[1] == npi) {
        ipi[1] = 0;
    }
    if (ipv[1] == npv) {
        ipv[1] = 0;
    }

    const float wti[2] = { wti0, wti1 };
    const float wtv[2] = { wtv0, wtv1 };
    const float wpi[2] = { wpi0, wpi1 };
    const float wpv[2] = { wpv0, wpv1 };

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    for (int i = 0; i < 2; ++i) {
        const float wi = wti[i];
        for (int j = 0; j < 2; ++j) {
            const float wij = wi * wtv[j];
            const std::size_t base_ij = nc * static_cast<std::size_t>(npi * iti[i]) + static_cast<std::size_t>(npv * itv[j]);
            for (int k = 0; k < 2; ++k) {
                const float wijk = wij * wpi[k];
                const std::size_t base_ijk = base_ij + nc * static_cast<std::size_t>(ipi[k]);
                for (int l = 0; l < 2; ++l) {
                    const float w = wijk * wpv[l];
                    const std::size_t idx = base_ijk + static_cast<std::size_t>(ipv[l]);

                    r += Bd[idx] * w;
                    g += Bd[plane_stride + idx] * w;
                    b += Bd[2 * plane_stride + idx] * w;
                }
            }
        }
    }

    return { r, g, b };
}

std::pair<float, float> BRDF::getSphericalAngles(const vec3& vec) const {
    

    float theta = std::acos(std::clamp(vec.z, -1.0f, 1.0f)); // Elevation angle
    float phi = std::atan2(vec.y, vec.x); // Azimuth angle
    if (phi < 0.0f) 
        phi += 2.0f * PI; // Ensure phi is in [0, 2π]

    return { theta, phi };
}