#include "objLoader.h"

sObjLoader::sObjLoader(){

    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = cfg.MAT_PATH; // Path to material files
    
    tinyobj::ObjReader reader;
    
    if (!reader.ParseFromFile( cfg.SCENE_PATH, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    readMaterials(materials);
    std::vector<uint32_t> lit_triangles;
    
    readTriangles(attrib, shapes, lit_triangles);
    readLightTriangles(lit_triangles);
    
    logger._cntTriangles = _sceneTriangles.size();
    logger._cntMaterials = _sceneMaterials.size();
    logger._cntLights = _lightTriangles.size();
}

void sObjLoader::readTriangles(const tinyobj::attrib_t& attrib, const std::vector<tinyobj::shape_t>& shapes, std::vector<uint32_t>& lit_triangles) {
    uint32_t tri_idx = 0;
    // Save tris to scene DS
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            sTriangle tri;
            bool hasNormals = true;
            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
                tri._vertices[v] = vec3(vx, vy, vz);

                // Update scene AABB
                sceneAABB.update(tri._vertices[v]);

                //centroid
                tri._centeroid += tri._vertices[v];
                
                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                    tri._normals[v] = vec3(nx, ny, nz);
                }
                else {
                    hasNormals = false;
                    tri._normals[v] = vec3();
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1]; 
                    tri._uvs[v] = vec3(tx, ty, 0.0f);
                }
                else {
                    tri._uvs[v] = vec3();
                }
            }

            tri._aabb = sAABB(tri._vertices);
            tri._centeroid = tri._centeroid / 3.0f;
            tri._matIdx = shapes[s].mesh.material_ids[f];

            computeNormalsUVsTangents(tri, hasNormals);

            _sceneTriangles.emplace_back(tri);

            if(_sceneMaterials[tri._matIdx]._emission.x > 0.0f ||
                _sceneMaterials[tri._matIdx]._emission.y > 0.0f ||
                _sceneMaterials[tri._matIdx]._emission.z > 0.0f)
                lit_triangles.push_back(tri_idx);
            
            ++tri_idx;
            index_offset += fv;
        }
    }
}

void sObjLoader::computeNormalsUVsTangents(sTriangle& tri, bool hasNormals) {
    // Compute face normal if not provided in the obj file
    vec3 a = tri._vertices[0], b = tri._vertices[1], c = tri._vertices[2];
    vec3 edge1 = b - a;
    vec3 edge2 = c - a;
    vec3 faceNormal = cross(edge1, edge2);

    if(!hasNormals) {
        faceNormal = faceNormal.normalize();
        tri._normals[0] = faceNormal;
        tri._normals[1] = faceNormal;
        tri._normals[2] = faceNormal;
    }
    
    // TexScale compute
    vec3 uv0 = tri._uvs[0], uv1 = tri._uvs[1], uv2 = tri._uvs[2];
    float edgeU1 = uv1.x - uv0.x;
    float edgeV1 = uv1.y - uv0.y;
    float edgeU2 = uv2.x - uv0.x;
    float edgeV2 = uv2.y - uv0.y;

    float geoArea = faceNormal.length() * 0.5f;
    float uvArea = std::abs((edgeU1 * edgeV2 - edgeV1 * edgeU2) * 0.5f);
    float texScale = geoArea > gEpsilon ? std::sqrt(uvArea / geoArea) : 1.0f;
    tri._texScale = texScale;

    vec3 deltaUV1 = uv1 - uv0;
    vec3 deltaUV2 = uv2 - uv0;
    float det = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;

    if (std::abs(det) < 1e-8f)
    {
        tri._tangents[0] = vec3(1,0,0);
        tri._tangents[1] = vec3(1,0,0);
        tri._tangents[2] = vec3(1,0,0);
        return;
    }

    float denom = 1.0f / det;

    float tX = denom * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    float tY = denom * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    float tZ = denom * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    vec3 flatTangent(tX, tY, tZ);
    flatTangent = flatTangent.normalize();

    for (uint32_t i = 0; i < DIM; ++i) {
        vec3 n = tri._normals[i];
        // Gram-Schmidt: T = T - N * dot(N, T)
        vec3 t = flatTangent - n * dot(n, flatTangent);
        tri._tangents[i] = t.normalize();
    }

}

void sObjLoader::readMaterials(const std::vector<tinyobj::material_t>& materials) {
    for (const auto& m : materials) {
        _sceneMaterials.emplace_back(vec3(m.diffuse[0], m.diffuse[1], m.diffuse[2]),
                                    vec3(m.specular[0], m.specular[1], m.specular[2]), 
                                    vec3(m.transmittance[0], m.transmittance[1], m.transmittance[2]), 
                                    vec3(m.emission[0], m.emission[1], m.emission[2]),
                                    m.shininess, m.ior);

        if (!m.diffuse_texname.empty()) { 
            std::cout << "Loading texture: " << m.diffuse_texname << std::endl; 
            _sceneMaterials.back()._albedoTex = std::make_shared<cTexture>(m.diffuse_texname);                        
        }
            
        if (!m.normal_texname.empty()) { 
            std::cout << "Loading texture: " << m.normal_texname << std::endl; 
            _sceneMaterials.back()._normalTex = std::make_shared<cTexture>(m.normal_texname, false);                 
        }
    }
}

void sObjLoader::readLightTriangles(const std::vector<uint32_t>& lit_triangles) {
    float light_area = 0.0f;
    vec3 emission;
    
    for (size_t i = 0; i < lit_triangles.size(); ++i){
        sTriangle tri = _sceneTriangles[lit_triangles[i]];
        vec3 a = tri._vertices[0], b = tri._vertices[1], c = tri._vertices[2];

        emission = _sceneMaterials[tri._matIdx]._emission;
        vec3 normal = cross(b - a, c - a);
        light_area = normal.length() * 0.5f;
        _lightTriangles.emplace_back(std::move(tri._vertices), normal.normalize(), emission, light_area);
    }
}