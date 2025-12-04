#pragma once
#include "../.preset/preset_data.h"
#include "buffer_data.h"
#include "../camera.h"
#include <emscripten/html5.h>
#include <vector>
#include <memory>
#include <unordered_map>

struct PlanetBuffer {
    PlanetData data;
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
    glm::vec3 worldPos;
    bool isPreview = false;

    PlanetBuffer() : 
        worldPos(0.0f),
        isPreview(false) 
    {}
    PlanetBuffer(PlanetBuffer&&) = default;
    PlanetBuffer& operator=(PlanetBuffer&&) = default;
    
    PlanetBuffer(const PlanetBuffer&) = delete;
    PlanetBuffer& operator=(const PlanetBuffer&) = delete;
};

class BufferGenerator {
    private:    
        Camera* camera;
        std::unordered_map<int, float> distanceMap;
        //glm::vec3 colorStringToVec3(const std::string& colorStr);

        void loadDistanceMap();
    public:
        BufferGenerator(Camera* camera);
        ~BufferGenerator();

        std::vector<PlanetBuffer> generateFromPreset(const PresetData& preset);
        PlanetBuffer generatePlanet(const PlanetData& data);
        void updatePlanetRotation(std::vector<PlanetBuffer>& planets, float deltaTime);
        int findAvailablePosition(const std::vector<PlanetData>& planets);
        bool replaceLastPlanet(std::vector<PlanetData>& planets, const PlanetData& newPlanet);
        float calculateDistanceFromPosition(int position);

        BufferData::Type shapeToBufferType(const std::string& name);
        RotationAxis rotationToBufferType(const std::string& axis);
};

#ifdef __cplusplus
extern "C" {
#endif
    void EMSCRIPTEN_KEEPALIVE generatePlanetParser(const char* data);
#ifdef __cplusplus
}
#endif