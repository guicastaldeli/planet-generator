#pragma once
#include "../buffers/buffer_data.h"
#include "../lightning/point_light.h"
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

    std::string lightning = "None";
    bool hasSunLight = false;
    PointLight sunLight;

    std::string effects = "None";
    int effectType = 0;
    
    // NEW: Store texture data for export/import
    std::string textureData = "";  // Base64 encoded texture data
    int textureWidth = 0;
    int textureHeight = 0;
};

struct PresetData {
    std::vector<PlanetData> planets;
    std::string name;
    std::string description;
    bool isDefault;
};

