/*!**************************************************************************
\file    TBRDF.h
\author  Jiri Filip, UTIA AS CR
\date    June 2013
\edited  Josef Jech, FIT CTU
\date    November 2026
\version 2.0 (C++23)

  The header file for:
   - the BRDF dataset loading from binary file (doubles) 
   - linear values interpolation for query incoming and outgoing directions
******************************************************************************/

#pragma once

#include <filesystem>
#include <span>
#include <string>
#include <vector>

#include "../math/vec3.h"

class BRDF {
public:
    BRDF() = default;

    explicit BRDF(const std::filesystem::path& filepath);

    /**
     * @brief Load BRDF binary dataset from file (reads 64-bit doubles, stores as 32-bit floats).
     * @param filepath Path to the .bin BRDF file.
     * @throws std::runtime_error If the file cannot be found, opened, read, or size is invalid.
     */
    void loadBrdf(const std::filesystem::path& filepath);

    /**
     * @brief Interpolate BRDF RGB value for given incoming and outgoing spherical angles.
     */
    [[nodiscard]] vec3 lookupBrdfVal(const vec3& illumination, const vec3& view) const;

    [[nodiscard]] bool isLoaded() const noexcept { return _isLoaded; }
    [[nodiscard]] const std::filesystem::path& path() const noexcept { return _filePath; }
    [[nodiscard]] const std::string& name() const noexcept { return _matName; }
    [[nodiscard]] std::span<const float> data() const noexcept { return Bd; }

private:
    static constexpr float step_t = 15.0f;
    static constexpr float step_p = 7.5f;
    static constexpr int nti = 6;
    static constexpr int ntv = 6;
    static constexpr int npi = static_cast<int>(360.0f / step_p);
    static constexpr int npv = static_cast<int>(360.0f / step_p);
    static constexpr int planes = 3;

    static constexpr std::size_t total_elements = static_cast<std::size_t>(planes * nti * npi * ntv * npv); // 248832
    static constexpr std::size_t nc = static_cast<std::size_t>(npv * ntv);
    static constexpr std::size_t nr = static_cast<std::size_t>(npi * nti); 
    static constexpr std::size_t plane_stride = nr * nc;                 

    static constexpr float inv_step_t = 1.0f / step_t;
    static constexpr float inv_step_p = 1.0f / step_p;

    std::filesystem::path _filePath{};
    std::string _matName{};
    bool _isLoaded = false;
    std::vector<float> Bd{};

    /** @brief Compute spherical angles for a given vector.
     *  @param vec Input vector.
     *  @return Pair of elevation and azimuth angles in radians.
     */
    std::pair<float, float> getSphericalAngles(const vec3& vec) const;

};
