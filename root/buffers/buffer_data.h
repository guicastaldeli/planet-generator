#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp> 

class BufferData {
    public:
        enum class Type {
            TRIANGLE,
            CUBE,
            SPHERE
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
                    -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
                    0.5f, -0.5f, -0.5f,      1.0f, 0.0f,
                    0.5f, -0.5f,  0.5f,      1.0f, 1.0f,
                    -0.5f, -0.5f,  0.5f,     0.0f, 1.0f,
                    0.0f,  0.5f,  0.0f,      0.5f, 0.5f
                },
                {
                    4, 0, 1,
                    4, 1, 2,
                    4, 2, 3,
                    4, 3, 0,
                    0, 1, 2,
                    0, 2, 3
                },
                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3(0.5f, 0.5f, 0.5f)
            });
            /* Cube */
            map.emplace(Type::CUBE, MeshData{
                {
                    -0.5f, -0.5f,  0.5f,     0.0f, 0.0f,
                    0.5f, -0.5f,  0.5f,      1.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,      1.0f, 1.0f,
                    -0.5f,  0.5f,  0.5f,     0.0f, 1.0f,
                    
                    0.5f, -0.5f, -0.5f,      0.0f, 0.0f,
                    -0.5f, -0.5f, -0.5f,     1.0f, 0.0f,
                    -0.5f,  0.5f, -0.5f,     1.0f, 1.0f,
                    0.5f,  0.5f, -0.5f,      0.0f, 1.0f,
                    
                    -0.5f,  0.5f,  0.5f,     0.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,      1.0f, 0.0f,
                    0.5f,  0.5f, -0.5f,      1.0f, 1.0f,
                    -0.5f,  0.5f, -0.5f,     0.0f, 1.0f,
                    
                    -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
                    0.5f, -0.5f, -0.5f,      1.0f, 0.0f,
                    0.5f, -0.5f,  0.5f,      1.0f, 1.0f,
                    -0.5f, -0.5f,  0.5f,     0.0f, 1.0f,
                    
                    0.5f, -0.5f,  0.5f,      0.0f, 0.0f,
                    0.5f, -0.5f, -0.5f,      1.0f, 0.0f,
                    0.5f,  0.5f, -0.5f,      1.0f, 1.0f,
                    0.5f,  0.5f,  0.5f,      0.0f, 1.0f,
                    
                    -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
                    -0.5f, -0.5f,  0.5f,     1.0f, 0.0f,
                    -0.5f,  0.5f,  0.5f,     1.0f, 1.0f,
                    -0.5f,  0.5f, -0.5f,     0.0f, 1.0f
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
            /* Sphere */
            auto sphereData = generateSphere(12);
            map.emplace(Type::SPHERE, std::move(sphereData));

            return map;
        }

        static MeshData generateSphere(int subdivisions) {
            std::vector<float> vertices;
            std::vector<GLuint> indices;

            std::vector<glm::vec3> cubeVertices = {
                { -0.5f, -0.5f, -0.5f },
                { 0.5f, -0.5f, -0.5f },
                { 0.5f, 0.5f, -0.5f },
                { -0.5f, 0.5f, -0.5f },
                { -0.5f, -0.5f, 0.5f },
                { 0.5f, -0.5f, 0.5f },
                { 0.5f, 0.5f, 0.5f },
                { -0.5f, 0.5f, 0.5f }
            };
            std::vector<std::vector<GLuint>> cubeFaces = {
                { 0, 1, 2, 3 },
                { 5, 4, 7, 6 },
                { 4, 0, 3, 7 },
                { 1, 5, 6, 2 },
                { 3, 2, 6, 7 },
                { 4, 5, 1, 0 }
            };

            for(int face = 0; face < 6; ++face) {
                const auto& faceIndices = cubeFaces[face];
                glm::vec3 v0 = cubeVertices[faceIndices[0]];
                glm::vec3 v1 = cubeVertices[faceIndices[1]];
                glm::vec3 v2 = cubeVertices[faceIndices[2]];
                glm::vec3 v3 = cubeVertices[faceIndices[3]];
                
                for(int y = 0; y <= subdivisions; ++y) {
                    float fy = static_cast<float>(y) / subdivisions;
                    glm::vec3 a = glm::mix(v0, v3, fy);
                    glm::vec3 b = glm::mix(v1, v2, fy);

                    for(int x = 0; x <= subdivisions; ++x) {
                        float fx = static_cast<float>(x) / subdivisions;
                        glm::vec3 point = glm::mix(a, b, fx);
                        point = glm::normalize(point) * 0.5f;
                        vertices.push_back(point.x);
                        vertices.push_back(point.y);
                        vertices.push_back(point.z);
                        
                        float u;
                        float v;
                        if(face == 0 || face == 1) {
                            u = (point.x + 0.5f);
                            v = (point.y + 0.5f);
                        } else if(face == 2 || face == 3) {
                            u = (point.z + 0.5f);
                            v = (point.y + 0.5f);
                        } else {
                            u = (point.x + 0.5f);
                            v = (point.z + 0.5f);
                        }
                        
                        u = glm::clamp(u, 0.0f, 1.0f);
                        v = glm::clamp(v, 0.0f, 1.0f);
                        vertices.push_back(u);
                        vertices.push_back(v);
                    }
                }
            }

            int verticesPerFace = (subdivisions + 1) * (subdivisions + 1);
            for(int face = 0; face < 6; ++face) {
                int baseVertex = face * verticesPerFace;
                for(int y = 0; y < subdivisions; ++y) {
                    for(int x = 0; x < subdivisions; ++x) {
                        int v0 = baseVertex + y * (subdivisions + 1) + x;
                        int v1 = v0 + 1;
                        int v2 = baseVertex + (y + 1) * (subdivisions + 1) + x;
                        int v3 = v2 + 1;

                        indices.push_back(v0);
                        indices.push_back(v2);
                        indices.push_back(v1);

                        indices.push_back(v1);
                        indices.push_back(v2);
                        indices.push_back(v3);
                    }
                }
            }

            glm::vec3 minBounds(-0.5f, -0.5f, -0.5f);
            glm::vec3 maxBounds(0.5f, 0.5f, 0.5f);
            return MeshData{ vertices, indices, minBounds, maxBounds };
        }
};