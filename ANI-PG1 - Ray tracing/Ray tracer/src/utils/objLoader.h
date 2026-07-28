#pragma once

/*
    objLoader.h
    Loader for OBJ scene geometry, materials, and emissive light triangles.
    Converts tinyobjloader data into the renderer's triangle and material
    structures.
*/

#include <iostream>
#include <vector>
#include <random>
#include <memory>

#include "../../external/tiny_obj_loader.h"
#include "../scene/texture.h"
#include "../scene/material.h"


struct sObjLoader{
    std::vector<sTriangle> _sceneTriangles;
    std::vector<sMaterial> _sceneMaterials;
    std::vector<sTriangleLight> _lightTriangles;
    
    /**
     * @brief Construct a new OBJ loader.
     */
    sObjLoader();

    /**
     * @brief Read triangle geometry from OBJ shapes.
     *
     * @param attrib OBJ vertex attribute arrays.
     * @param shapes OBJ mesh shapes.
     * @param lit_triangles Output list of light-emitting triangle indices.
     */
    void readTriangles(const tinyobj::attrib_t& attrib, const std::vector<tinyobj::shape_t>& shapes, std::vector<uint32_t>& lit_triangles);

    /**
     * @brief Compute normals, UVs, and tangent vectors for a triangle.
     *
     * @param tri Triangle to update.
     * @param hasNormals True if the model already supplied normals.
     */
    void computeNormalsUVsTangents(sTriangle& tri, bool hasNormals);

    /**
     * @brief Load material definitions from OBJ materials.
     *
     * @param materials List of parsed OBJ materials.
     */
    void readMaterials(const std::vector<tinyobj::material_t>& materials);

    /**
     * @brief Build light-emitting triangle primitives from light indices.
     *
     * @param lit_triangles Indices of triangles flagged as lights.
     */
    void readLightTriangles(const std::vector<uint32_t>& lit_triangles);
};