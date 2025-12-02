#include "buffer_generator.h"
#include "buffers.h"
#include "../camera.h"
#include "../.controller/shader_controller.h"
#include "../preset/preset_loader.h"
#include "../.data/data_parser.h"
#include <algorithm>
#include <queue>
#include <iostream>
#include <unordered_set>
#include <emscripten/html5.h>
#include <fstream>
#include <sstream>

BufferGenerator::BufferGenerator() {
    loadDistanceMap();
};
BufferGenerator::~BufferGenerator() {};

/*
** Load Distance Map
*/
void BufferGenerator::loadDistanceMap() {
    try {
        std::ifstream file("/.data/positions.json");
        if(!file.is_open()) {
            std::cerr << "Failed to open positions.json file" << std::endl;
            return;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string data = buffer.str();

        DataParser::Value root = DataParser::Parser::parse(data);
        if(!root.hasKey("distances") || !root["distances"].isArray()) {
            std::cerr << "Invalid positions.json format: missing distances array" << std::endl;
            return;
        }

        const auto& distancesArray = root["distances"].asArray();
        for(const auto& distanceValue : distancesArray) {
            if(!distanceValue.isObject()) continue;
            if(distanceValue.hasKey("index") && distanceValue.hasKey("distance")) {
                int i = distanceValue["index"].asInt();
                std::string distanceStr = distanceValue["distance"].asString();
                if(distanceStr.back() == 'f') {
                    distanceStr.pop_back();
                }

                float distance = std::stof(distanceStr);
                distanceMap[i] = distance;
            }
        }

        std::cout << "Loaded " << distanceMap.size() << " distance mappings" << std::endl;
    } catch(const std::exception& err) {
        std::cerr << "Error loading distance map: " << err.what() << std::endl;
    }
}

/*
** Shape to Buffer
*/
BufferData::Type BufferGenerator::shapeToBufferType(BufferData::Type shape) {
    switch(shape) {
        case BufferData::Type::TRIANGLE: 
            return BufferData::Type::TRIANGLE;
        case BufferData::Type::CUBE:
            return BufferData::Type::CUBE;
        case BufferData::Type::SPHERE:
            return BufferData::Type::SPHERE;
        default:
            return BufferData::Type::SPHERE;
    }
}

/*
** Calculate Distance from Pos
*/
float BufferGenerator::calculateDistanceFromPosition(int position) {
    auto it = distanceMap.find(position);
    if(it != distanceMap.end()) {
        return it->second;
    }
    return 1.0f + (position - 8) * 0.15f;
}

/*
** Generate Planets
*/
std::vector<PlanetBuffer> BufferGenerator::generateFromPreset(const PresetData& preset) {
    std::vector<PlanetBuffer> planetBuffers;
    for(const auto& data : preset.planets) {
        planetBuffers.push_back(generatePlanet(data));
    }
    return planetBuffers;
}

PlanetBuffer BufferGenerator::generatePlanet(const PlanetData& data) {
    PlanetBuffer planetBuffer;
    planetBuffer.data = data;
    return planetBuffer;
}

/*
** Update Planets
*/
void BufferGenerator::updatePlanetRotation(std::vector<PlanetBuffer>& planets, float deltaTime) {
    for(auto& planet : planets) {
        switch(planet.data.rotationDir) {
            case RotationAxis::X:
                planet.data.currentRotation.x += planet.data.rotationSpeedItself * deltaTime;
                break;
            case RotationAxis::Y:
                planet.data.currentRotation.y += planet.data.rotationSpeedItself * deltaTime;
                break;
            case RotationAxis::Z:
                planet.data.currentRotation.z += planet.data.rotationSpeedItself * deltaTime;
        }
        if(planet.data.position != 0) {
            planet.data.orbitAngle.y += planet.data.rotationSpeedCenter * deltaTime;
        }
    }
}

/*
** Find Available Position
*/
int BufferGenerator::findAvailablePosition(const std::vector<PlanetData>& planets) {
    const int MAX_PLANETS = 15;
    std::vector<bool> occupied(MAX_PLANETS, false);
    for(const auto& planet : planets) {
        if(planet.position >= 0 && planet.position < MAX_PLANETS) {
            occupied[planet.position] = true;
        }
    }

    std::queue<int> queue;
    queue.push(1);
    while(!queue.empty()) {
        int pos = queue.front();
        queue.pop();

        if(!occupied[pos]) return pos;
        if(pos + 1 < MAX_PLANETS) queue.push(pos + 1);
    }

    return -1;
}

/*
** Replace Last Planet
*/
bool BufferGenerator::replaceLastPlanet(
    std::vector<PlanetData>& planets,
    const PlanetData& newPlanet
) {
    if(planets.empty()) return false;

    const int MAX_PLANETS = 16;
    int highestPos = -1;
    size_t highestIndex = 0;
    for(size_t i = 0; i < planets.size(); i++) {
        if(planets[i].position != 0 && planets[i].position > highestPos) {
            highestPos  = planets[i].position;
            highestIndex = i;
        }
    }
    if(highestPos == -1) {
        return false;
    }

    planets[highestIndex] = newPlanet;
    planets[highestIndex].position = highestPos;
    return true;
};

/*
** Generate
*/
void generate(const char* data) {
    try {
        std::string str(data);

        PlanetData newPlanet;
        BufferGenerator bufferGenerator;
        PresetLoader* presetLoader;

        auto& planets = presetLoader->getCurrentPreset().planets;
        int availablePos = bufferGenerator.findAvailablePosition(
            presetLoader->getCurrentPreset().planets
        );
        if(availablePos == -1) {
            bufferGenerator.replaceLastPlanet(
                presetLoader->getCurrentPreset().planets, newPlanet
            );
        } else {
            newPlanet.position = availablePos;
            newPlanet.distanceFromCenter = bufferGenerator.calculateDistanceFromPosition(
                availablePos
            );
            presetLoader->getCurrentPreset().planets.push_back(newPlanet);
        }

        Buffers* buffers;
        auto newBuffers = bufferGenerator.generateFromPreset(
            presetLoader->getCurrentPreset()
        );
        buffers->planetBuffers.clear();
        for(auto& buffer : newBuffers) {
            buffers->planetBuffers.push_back(std::move(buffer));
        }
    } catch(const std::exception& err) {
        std::cerr << "Error creating planet!" << err.what() << std::endl;
    }
}