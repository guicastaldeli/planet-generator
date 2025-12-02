#pragma once
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include "buffer_data.h"
#include "../buffers/buffer_generator.h"
#include "../.controller/buffer_controller.h"

class Camera;
class ShaderController;
class Buffers {
    private:
        Camera* camera;
        ShaderController* shaderController;
        BufferController* bufferController;

        std::unordered_map<BufferData::Type, GLuint> vaos;
        std::unordered_map<BufferData::Type, GLuint> vbos;
        std::unordered_map<BufferData::Type, GLuint> ebos;
        std::unordered_map<BufferData::Type, size_t> indexCounts;

        glm::vec3 minBounds;
        glm::vec3 maxBounds;
        
        void set(BufferData::Type type);
        
    public:
        Buffers(
            Camera* camera, 
            ShaderController* shaderController,
            BufferController* bufferController
        );
        ~Buffers();

        std::vector<PlanetBuffer> planetBuffers;

        glm::vec3 getMinBounds() const {
            return minBounds;
        }
        glm::vec3 getMaxBounds() const {
            return maxBounds;
        }

        void createBufferForPlanet(const PlanetBuffer& planetBuffer);
        void render();
        void init();
};