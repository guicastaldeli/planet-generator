#pragma once
#include "../.buffers/buffer_data.h"
#include <string>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

enum RotationAxis {
    X,
    Y,
    Z
};

struct PlanetData {
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
};

struct PresetData {
    std::vector<PlanetData> planets;
    std::string name;
    std::string description;
    bool isDefault;
};

