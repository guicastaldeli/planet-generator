#pragma once
#include "../.preset/preset_data.h"
#include <glm/glm.hpp>

class PresetManager;
class DefaultData {
    public:
        struct Data {
            uint32_t id;
            std::string name;
            BufferData::Type shape;
            float size;
            std::string texture;
            std::string color;
            glm::vec3 colorRgb;
            int position;
            RotationAxis rotationDir;
            float rotationSpeedItself;
            float rotationSpeedCenter;
            float distanceFromCenter;
            glm::vec3 currentRotation;
            glm::vec3 orbitAngle;

            PlanetData toPlanetData() const;
        };

        DefaultData(PresetManager* presetManager);
        ~DefaultData();

        const std::vector<Data>& getAllData() const;
        void init();

    private:
        PresetManager* presetManager;
        std::vector<Data> defaultData;

        std::string path;

        void setPath();
        bool setData();
};
