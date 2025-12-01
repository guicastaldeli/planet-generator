#pragma once
#include "../preset/preset_data.h"
#include "buffer_data.h"
#include <vector>
#include <memory>

class Buffers;

struct PlanetBuffer {
    std::unique_ptr<Buffers> buffer;
    PlanetData data;
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
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

    private:
        BufferData::Type shapeToBufferType(BufferData::Type shape);
        //glm::vec3 colorStringToVec3(const std::string& colorStr);
        float calculateDistanceFromPosition(int position);
};