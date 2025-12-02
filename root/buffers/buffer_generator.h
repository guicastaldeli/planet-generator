#pragma once
#include "../preset/preset_data.h"
#include "buffer_data.h"
#include <emscripten/html5.h>
#include <vector>
#include <memory>
#include <unordered_map>

class Buffers;

struct PlanetBuffer {
    std::unique_ptr<Buffers> buffer;
    PlanetData data;
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;

    PlanetBuffer() = default;
    PlanetBuffer(PlanetBuffer&&) = default;
    PlanetBuffer& operator=(PlanetBuffer&&) = default;
    
    PlanetBuffer(const PlanetBuffer&) = delete;
    PlanetBuffer& operator=(const PlanetBuffer&) = delete;
};

class BufferGenerator {
    public:
        BufferGenerator();
        ~BufferGenerator();

        std::vector<PlanetBuffer> generateFromPreset(const PresetData& preset);
        PlanetBuffer generatePlanet(const PlanetData& data);
        void updatePlanetRotation(std::vector<PlanetBuffer>& planets, float deltaTime);
        int findAvailablePosition(const std::vector<PlanetData>& planets);
        bool replaceLastPlanet(std::vector<PlanetData>& planets, const PlanetData& newPlanet);
        float calculateDistanceFromPosition(int position);

    private:
        std::unordered_map<int, float> distanceMap;
        BufferData::Type shapeToBufferType(BufferData::Type shape);
        //glm::vec3 colorStringToVec3(const std::string& colorStr);

        void loadDistanceMap();
};

#ifdef __cplusplus
extern "C" {
#endif
    void EMSCRIPTEN_KEEPALIVE generate(const char* data);
#ifdef __cplusplus
}
#endif