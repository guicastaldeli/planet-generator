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
            static const std::unordered_map<Type, MeshData> map = {
                /* Triangle */
                {Type::TRIANGLE, {
                    {
                        -0.5f, -0.5f, -0.5f,
                        0.5f, -0.5f, -0.5f,
                        0.5f, -0.5f,  0.5f,
                        -0.5f, -0.5f,  0.5f,
                        0.0f,  0.5f,  0.0f
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
                }},
                /* Cube */
                {Type::CUBE, {
                    {
                        -0.5f, -0.5f, -0.5f,
                        0.5f, -0.5f, -0.5f,
                        0.5f,  0.5f, -0.5f,
                        -0.5f,  0.5f, -0.5f,
                        -0.5f, -0.5f,  0.5f,
                        0.5f, -0.5f,  0.5f,
                        0.5f,  0.5f,  0.5f,
                        -0.5f,  0.5f,  0.5f
                    },
                    {
                        0,1,2, 0,2,3,
                        4,5,6, 4,6,7,
                        0,4,7, 0,7,3,
                        1,5,6, 1,6,2,
                        3,2,6, 3,6,7,
                        0,1,5, 0,5,4
                    },
                    glm::vec3(-0.5f, -0.5f, -0.5f),
                    glm::vec3(0.5f, 0.5f, 0.5f)  
                }},
                /* Sphere */
                {Type::SPHERE, {
                    {
                        -0.5f, -0.5f, -0.5f,
                        0.5f, -0.5f, -0.5f,
                        0.5f, -0.5f,  0.5f,
                        -0.5f, -0.5f,  0.5f,
                        0.0f,  0.5f,  0.0f
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
                }}
            };

            return map.at(t);
        }
};