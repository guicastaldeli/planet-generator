#pragma once
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include "buffer_data.h"

class Camera;
class ShaderController;
class Buffers {
    private:
        Camera* camera;
        ShaderController* shaderController;

        GLuint vao;
        GLuint vbo;
        GLuint ebo;

        BufferData::Type bufferType;
        GLuint indexCount;

        glm::vec3 minBounds;
        glm::vec3 maxBounds;
        
        void set();
        
    public:
        Buffers(
            Camera* camera, 
            ShaderController* shaderController,
            BufferData::Type type
        );
        ~Buffers();

        glm::vec3 getMinBounds() const {
            return minBounds;
        }
        glm::vec3 getMaxBounds() const {
            return maxBounds;
        }

        void render();
        void init();
};