#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <GL/glew.h>

class BufferTypes {
    public:
        enum class Type {
            TRIANGLE,
            CUBE,
            SPHERE
        };

        struct MeshData {
            std::vector<float> vertices;
            std::vector<GLuint> indices;
        };

        static MeshData GetMeshData(Type t) {
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
                    }
                }},
                /* Cube */
                {Type::CUBE, {
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
                    }
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
                    }
                }}
            };

            return map.at(t);
        }
};