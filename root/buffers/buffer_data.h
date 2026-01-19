#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <GLES3/gl3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/constants.hpp>

class BufferData {
    public:
        enum class Type {
            TRIANGLE,
            CUBE,
            SPHERE,
            RING
        };

        struct MeshData {
            std::vector<float> vertices;
            std::vector<GLuint> indices;
            glm::vec3 minBounds;
            glm::vec3 maxBounds;

            MeshData(
                const std::vector<float>& v,
                const std::vector<GLuint>& i,
                const glm::vec3& min,
                const glm::vec3& max
            ) :
            vertices(v),
            indices(i),
            minBounds(min),
            maxBounds(max) {}
        };

        static const MeshData& GetMeshData(Type t) {
            static const std::unordered_map<Type, MeshData> map = Data();
            return map.at(t);
        }
    private:
        static std::unordered_map<Type, MeshData> Data() {
            std::unordered_map<Type, MeshData> map;
            
            /* Triangle */
            map.emplace(Type::TRIANGLE, MeshData{
                {
                    -0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f,     0.0f, 0.0f,
                    0.5f, -0.5f, -0.5f,      0.0f, -1.0f, 0.0f,     1.0f, 0.0f,
                    0.5f, -0.5f,  0.5f,      0.0f, -1.0f, 0.0f,     1.0f, 1.0f,
                    -0.5f, -0.5f,  0.5f,     0.0f, -1.0f, 0.0f,     0.0f, 1.0f,
                    
                    0.0f,  0.5f,  0.0f,      -0.894f, 0.447f, 0.0f,   0.5f, 0.5f,
                    0.0f,  0.5f,  0.0f,      0.0f, 0.447f, -0.894f,   0.5f, 0.5f, 
                    0.0f,  0.5f,  0.0f,      0.894f, 0.447f, 0.0f,   0.5f, 0.5f,
                    0.0f,  0.5f,  0.0f,      0.0f, 0.447f, 0.894f,   0.5f, 0.5f,
                },
                {
                    4, 0, 1,
                    5, 1, 2,
                    6, 2, 3,
                    7, 3, 0,
                    
                    0, 1, 2,
                    0, 2, 3
                },
                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3(0.5f, 0.5f, 0.5f)
            });
                        
            /* Cube */
            map.emplace(Type::CUBE, MeshData{
                {
                    -0.5f, -0.5f,  0.5f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
                    0.5f, -0.5f,  0.5f,      0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,      0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
                    -0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
                    
                    0.5f, -0.5f, -0.5f,      0.0f, 0.0f, -1.0f,    0.0f, 0.0f,
                    -0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,    1.0f, 0.0f,
                    -0.5f,  0.5f, -0.5f,     0.0f, 0.0f, -1.0f,    1.0f, 1.0f,
                    0.5f,  0.5f, -0.5f,      0.0f, 0.0f, -1.0f,    0.0f, 1.0f,
                    
                    -0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,      0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
                    0.5f,  0.5f, -0.5f,      0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
                    -0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
                    
                    -0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f,    0.0f, 0.0f,
                    0.5f, -0.5f, -0.5f,      0.0f, -1.0f, 0.0f,    1.0f, 0.0f,
                    0.5f, -0.5f,  0.5f,      0.0f, -1.0f, 0.0f,    1.0f, 1.0f,
                    -0.5f, -0.5f,  0.5f,     0.0f, -1.0f, 0.0f,    0.0f, 1.0f,
                    
                    0.5f, -0.5f,  0.5f,      1.0f, 0.0f, 0.0f,     0.0f, 0.0f,
                    0.5f, -0.5f, -0.5f,      1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
                    0.5f,  0.5f, -0.5f,      1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
                    0.5f,  0.5f,  0.5f,      1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
                    
                    -0.5f, -0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,     0.0f, 0.0f,
                    -0.5f, -0.5f,  0.5f,    -1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
                    -0.5f,  0.5f,  0.5f,    -1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
                    -0.5f,  0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,     0.0f, 1.0f
                },
                {
                    0, 1, 2, 0, 2, 3,
                    4, 5, 6, 4, 6, 7,
                    8, 9, 10, 8, 10, 11,
                    12, 13, 14, 12, 14, 15,
                    16, 17, 18, 16, 18, 19,
                    20, 21, 22, 20, 22, 23
                },
                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3(0.5f, 0.5f, 0.5f)
            });
            
            /* Sphere - Now using Icosphere with proper UV seam handling */
            auto sphereData = generateIcosphere(3);
            map.emplace(Type::SPHERE, std::move(sphereData));

            /* Ring */
            map.emplace(Type::RING, MeshData{
                generateRingVertices(),
                generateRingIndices(),
                glm::vec3(-0.6f, -0.1f, -0.6f),
                glm::vec3(0.6f, 0.1f, 0.6f)
            });

            return map;
        }

        static MeshData generateIcosphere(int subdivisions) {
            std::vector<glm::vec3> positions;
            std::vector<GLuint> indices;
            
            // Create icosahedron base
            const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
            
            positions = {
                glm::normalize(glm::vec3(-1,  t,  0)),
                glm::normalize(glm::vec3( 1,  t,  0)),
                glm::normalize(glm::vec3(-1, -t,  0)),
                glm::normalize(glm::vec3( 1, -t,  0)),
                
                glm::normalize(glm::vec3( 0, -1,  t)),
                glm::normalize(glm::vec3( 0,  1,  t)),
                glm::normalize(glm::vec3( 0, -1, -t)),
                glm::normalize(glm::vec3( 0,  1, -t)),
                
                glm::normalize(glm::vec3( t,  0, -1)),
                glm::normalize(glm::vec3( t,  0,  1)),
                glm::normalize(glm::vec3(-t,  0, -1)),
                glm::normalize(glm::vec3(-t,  0,  1))
            };
            
            indices = {
                0, 11, 5,   0, 5, 1,    0, 1, 7,    0, 7, 10,   0, 10, 11,
                1, 5, 9,    5, 11, 4,   11, 10, 2,  10, 7, 6,   7, 1, 8,
                3, 9, 4,    3, 4, 2,    3, 2, 6,    3, 6, 8,    3, 8, 9,
                4, 9, 5,    2, 4, 11,   6, 2, 10,   8, 6, 7,    9, 8, 1
            };
            
            // Subdivide
            std::map<std::pair<GLuint, GLuint>, GLuint> midpointCache;
            
            auto getMidpoint = [&](GLuint i1, GLuint i2) -> GLuint {
                std::pair<GLuint, GLuint> key = i1 < i2 ? std::make_pair(i1, i2) : std::make_pair(i2, i1);
                
                auto it = midpointCache.find(key);
                if (it != midpointCache.end()) {
                    return it->second;
                }
                
                glm::vec3 point1 = positions[i1];
                glm::vec3 point2 = positions[i2];
                glm::vec3 middle = glm::normalize((point1 + point2) * 0.5f);
                
                GLuint index = positions.size();
                positions.push_back(middle);
                midpointCache[key] = index;
                
                return index;
            };
            
            for (int i = 0; i < subdivisions; ++i) {
                std::vector<GLuint> newIndices;
                
                for (size_t j = 0; j < indices.size(); j += 3) {
                    GLuint v1 = indices[j];
                    GLuint v2 = indices[j + 1];
                    GLuint v3 = indices[j + 2];
                    
                    GLuint a = getMidpoint(v1, v2);
                    GLuint b = getMidpoint(v2, v3);
                    GLuint c = getMidpoint(v3, v1);
                    
                    newIndices.push_back(v1); newIndices.push_back(a); newIndices.push_back(c);
                    newIndices.push_back(v2); newIndices.push_back(b); newIndices.push_back(a);
                    newIndices.push_back(v3); newIndices.push_back(c); newIndices.push_back(b);
                    newIndices.push_back(a);  newIndices.push_back(b); newIndices.push_back(c);
                }
                
                indices = newIndices;
                midpointCache.clear();
            }
            
            // Build final vertex data with per-triangle vertices to handle UV seams
            std::vector<float> vertices;
            std::vector<GLuint> finalIndices;
            
            for (size_t i = 0; i < indices.size(); i += 3) {
                glm::vec3 v0 = positions[indices[i]];
                glm::vec3 v1 = positions[indices[i + 1]];
                glm::vec3 v2 = positions[indices[i + 2]];
                
                // Calculate UVs for each vertex
                float u0 = 0.5f + atan2(v0.z, v0.x) / (2.0f * glm::pi<float>());
                float v0_coord = 0.5f - asin(v0.y) / glm::pi<float>();
                
                float u1 = 0.5f + atan2(v1.z, v1.x) / (2.0f * glm::pi<float>());
                float v1_coord = 0.5f - asin(v1.y) / glm::pi<float>();
                
                float u2 = 0.5f + atan2(v2.z, v2.x) / (2.0f * glm::pi<float>());
                float v2_coord = 0.5f - asin(v2.y) / glm::pi<float>();
                
                // Fix UV seam (when triangle crosses 0/1 boundary)
                float uvs[3] = {u0, u1, u2};
                for (int j = 0; j < 3; ++j) {
                    for (int k = j + 1; k < 3; ++k) {
                        if (std::abs(uvs[j] - uvs[k]) > 0.5f) {
                            if (uvs[j] < 0.5f) uvs[j] += 1.0f;
                            else if (uvs[k] < 0.5f) uvs[k] += 1.0f;
                        }
                    }
                }
                u0 = uvs[0];
                u1 = uvs[1];
                u2 = uvs[2];
                
                GLuint baseIdx = vertices.size() / 8;
                
                // Vertex 0
                vertices.push_back(v0.x * 0.5f);
                vertices.push_back(v0.y * 0.5f);
                vertices.push_back(v0.z * 0.5f);
                vertices.push_back(v0.x);
                vertices.push_back(v0.y);
                vertices.push_back(v0.z);
                vertices.push_back(u0);
                vertices.push_back(v0_coord);
                
                // Vertex 1
                vertices.push_back(v1.x * 0.5f);
                vertices.push_back(v1.y * 0.5f);
                vertices.push_back(v1.z * 0.5f);
                vertices.push_back(v1.x);
                vertices.push_back(v1.y);
                vertices.push_back(v1.z);
                vertices.push_back(u1);
                vertices.push_back(v1_coord);
                
                // Vertex 2
                vertices.push_back(v2.x * 0.5f);
                vertices.push_back(v2.y * 0.5f);
                vertices.push_back(v2.z * 0.5f);
                vertices.push_back(v2.x);
                vertices.push_back(v2.y);
                vertices.push_back(v2.z);
                vertices.push_back(u2);
                vertices.push_back(v2_coord);
                
                finalIndices.push_back(baseIdx);
                finalIndices.push_back(baseIdx + 1);
                finalIndices.push_back(baseIdx + 2);
            }
            
            glm::vec3 minBounds(-0.5f, -0.5f, -0.5f);
            glm::vec3 maxBounds(0.5f, 0.5f, 0.5f);
            
            return MeshData{ vertices, finalIndices, minBounds, maxBounds };
        }

        static std::vector<float> generateRingVertices() {
            std::vector<float> vertices;
            int segments = 32;
            float innerRadius = 0.5f;
            float outerRadius = 0.6f;
            float thickness = 0.02f;

            for(int i = 0; i <= segments; ++i) {
                float angle = 2.0f * glm::pi<float>() * i / segments;
                float cosA = cos(angle);
                float sinA = sin(angle);

                vertices.push_back(cosA * outerRadius);
                vertices.push_back(thickness);
                vertices.push_back(sinA * outerRadius);
                vertices.push_back(0.0f);
                vertices.push_back(1.0f); 
                vertices.push_back(0.0f);
                vertices.push_back((float)i / segments);
                vertices.push_back(1.0f);

                vertices.push_back(cosA * outerRadius);
                vertices.push_back(-thickness);
                vertices.push_back(sinA * outerRadius);
                vertices.push_back(0.0f);
                vertices.push_back(-1.0f);
                vertices.push_back(0.0f);
                vertices.push_back((float)i / segments);
                vertices.push_back(0.0f);

                vertices.push_back(cosA * innerRadius);
                vertices.push_back(thickness);
                vertices.push_back(sinA * innerRadius);
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
                vertices.push_back(0.0f);
                vertices.push_back((float)i / segments);
                vertices.push_back(1.0f);

                vertices.push_back(cosA * innerRadius);
                vertices.push_back(-thickness);
                vertices.push_back(sinA * innerRadius);
                vertices.push_back(0.0f);
                vertices.push_back(-1.0f);
                vertices.push_back(0.0f);
                vertices.push_back((float)i / segments);
                vertices.push_back(0.0f);
            }

            return vertices;
        }

        static std::vector<GLuint> generateRingIndices() {
            std::vector<GLuint> indices;
            int segments = 32;

            for(int i = 0; i < segments; ++i) {
                int base = i * 4;
                int nextBase = ((i + 1) % segments) * 4;

                indices.push_back(base);
                indices.push_back(base + 1);
                indices.push_back(nextBase);
                indices.push_back(nextBase);
                indices.push_back(base + 1);
                indices.push_back(nextBase + 1);

                indices.push_back(base + 2);
                indices.push_back(nextBase + 2);
                indices.push_back(base + 3);
                indices.push_back(nextBase + 2);
                indices.push_back(nextBase + 3);
                indices.push_back(base + 3);

                indices.push_back(base);
                indices.push_back(nextBase);
                indices.push_back(base + 2);
                indices.push_back(nextBase);
                indices.push_back(nextBase + 2);
                indices.push_back(base + 2);

                indices.push_back(base + 1);
                indices.push_back(base + 3);
                indices.push_back(nextBase + 1);
                indices.push_back(nextBase + 1);
                indices.push_back(base + 3);
                indices.push_back(nextBase + 3);
            }

            return indices;
        }
};