#include "buffer_generator.h"
#include "buffers.h"
#include "../camera.h"
#include "../.controller/shader_controller.h"
#include "../.preset/preset_loader.h"
#include "../_data/data_parser.h"
#include <algorithm>
#include <queue>
#include <iostream>
#include <unordered_set>
#include <emscripten/html5.h>
#include <fstream>
#include <sstream>

BufferGenerator::BufferGenerator(Camera* camera) :
    camera(camera) 
{
    loadDistanceMap();
};
BufferGenerator::~BufferGenerator() {};

/*
** Load Distance Map
*/
void BufferGenerator::loadDistanceMap() {
    try {
        std::ifstream file("/_data/positions.json");
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
** Shape to Buffer Type
*/
BufferData::Type BufferGenerator::shapeToBufferType(const std::string& name) {
    static const std::unordered_map<std::string, BufferData::Type> map = {
        { "SPHERE", BufferData::Type::SPHERE },
        { "CUBE", BufferData::Type::CUBE },
        { "TRIANGLE", BufferData::Type::TRIANGLE }
    };

    auto it = map.find(name);
    return it != map.end() ? it->second : BufferData::Type::SPHERE;
}

/*
** Rotation to Buffer Type
*/
RotationAxis BufferGenerator::rotationToBufferType(const std::string& axis) {
    static const std::unordered_map<std::string, RotationAxis> map = {
        { "X", RotationAxis::X },
        { "Y", RotationAxis::Y },
        { "Z", RotationAxis::Z }
    };

    auto it = map.find(axis);
    return it != map.end() ? it->second : RotationAxis::Y;
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
    const float SPEED_MULTIPLIER_CENTER = 2000.0f;
    const float SPEED_MULTIPLIER_ITSELF = 20.0f;
    
    for(auto& planet : planets) {
        switch(planet.data.rotationDir) {
            case RotationAxis::X:
                planet.data.currentRotation.x += planet.data.rotationSpeedItself * SPEED_MULTIPLIER_ITSELF * deltaTime;
                break;
            case RotationAxis::Y:
                planet.data.currentRotation.y += planet.data.rotationSpeedItself * SPEED_MULTIPLIER_ITSELF * deltaTime;
                break;
            case RotationAxis::Z:
                planet.data.currentRotation.z += planet.data.rotationSpeedItself * SPEED_MULTIPLIER_ITSELF * deltaTime;
                break;
        }
        if(planet.data.position != 0) {
            planet.data.orbitAngle.y += planet.data.rotationSpeedCenter * SPEED_MULTIPLIER_CENTER * deltaTime;
            planet.data.orbitAngle.y = fmod(planet.data.orbitAngle.y, 360.0f);
        }
        
        planet.data.currentRotation.x = fmod(planet.data.currentRotation.x, 360.0f);
        planet.data.currentRotation.y = fmod(planet.data.currentRotation.y, 360.0f);
        planet.data.currentRotation.z = fmod(planet.data.currentRotation.z, 360.0f);
        planet.data.orbitAngle.y = fmod(planet.data.orbitAngle.y, 360.0f);
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
        BufferGenerator* bufferGenerator;
        PresetLoader* presetLoader;

        auto& planets = presetLoader->getCurrentPreset().planets;
        int availablePos = bufferGenerator->findAvailablePosition(
            presetLoader->getCurrentPreset().planets
        );
        if(availablePos == -1) {
            bufferGenerator->replaceLastPlanet(
                presetLoader->getCurrentPreset().planets, newPlanet
            );
        } else {
            newPlanet.position = availablePos;
            newPlanet.distanceFromCenter = bufferGenerator->calculateDistanceFromPosition(
                availablePos
            );
            presetLoader->getCurrentPreset().planets.push_back(newPlanet);
        }

        Buffers* buffers;
        auto newBuffers = bufferGenerator->generateFromPreset(
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