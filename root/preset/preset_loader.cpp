#include "preset_loader.h"
#include "preset_data.h"
#include "../.data/data_parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

PresetLoader::PresetLoader(const std::string& path) :
    defaultPresetPath(path) 
{};
PresetLoader::~PresetLoader() {};

/*
** Parse
*/
bool PresetLoader::parse(const std::string& data) {
    try {
        DataParser::Value root = DataParser::Parser::parse(data);
        if(!root.hasKey("planets") || !root["planets"].isArray()) {
            std::cerr << "Invalid preset fomat, missing planets!" << std::endl;
            return false;
        }

        currentPreset.planets.clear();
        const auto& planetsArray = root["planets"].asArray();
        for(const auto& planetValue : planetsArray) {
            if(!planetValue.isObject()) continue;
            PlanetData planet;

            /* Props */
            if(planetValue.hasKey("id")) planet.id = planetValue["id"].asInt();
            if(planetValue.hasKey("name")) planet.name = planetValue["name"].asString();
            if(planetValue.hasKey("size")) planet.size = planetValue["size"].asFloat();
            if(planetValue.hasKey("color")) planet.color = planetValue["color"].asString();
            if(planetValue.hasKey("position")) planet.position = planetValue["position"].asInt();
            if(planetValue.hasKey("distanceFromCenter")) 
                planet.distanceFromCenter = planetValue["distanceFromCenter"].asFloat();
            if(planetValue.hasKey("rotationSpeedItself")) 
                planet.rotationSpeedItself = planetValue["rotationSpeedItself"].asFloat();
            if(planetValue.hasKey("rotationSpeedCenter")) 
                planet.rotationSpeedCenter = planetValue["rotationSpeedCenter"].asFloat();

            /* Shape */
            if(planetValue.hasKey("shape")) {
                std::string shapeStr = planetValue["shape"].asString();
                if(shapeStr == "SPHERE") planet.shape = BufferData::Type::SPHERE;
                else if(shapeStr == "CUBE") planet.shape = BufferData::Type::CUBE;
                else if(shapeStr == "TRIANGLE") planet.shape = BufferData::Type::TRIANGLE;
                else planet.shape = BufferData::Type::SPHERE;
            }

            /* Rotation */
            if(planetValue.hasKey("rotationDir")) {
                std::string rotStr = planetValue["rotationDir"].asString();
                if(rotStr == "X") planet.rotationDir = RotationAxis::X;
                else if(rotStr == "Y") planet.rotationDir = RotationAxis::Y;
                else if(rotStr == "Z") planet.rotationDir = RotationAxis::Z;
                else planet.rotationDir = RotationAxis::Y;
            }
            if(planetValue.hasKey("currentRotation") && planetValue["currentRotation"].isObject()) {
                const auto& rot = planetValue["currentRotation"];
                if(rot.hasKey("x")) planet.currentRotation.x = rot["x"].asFloat();
                if(rot.hasKey("y")) planet.currentRotation.y = rot["y"].asFloat();
                if(rot.hasKey("z")) planet.currentRotation.z = rot["z"].asFloat();
            }

            /* Orbit Angle */
            if(planetValue.hasKey("orbitAngle") && planetValue["orbitAngle"].isObject()) {
                const auto& orbit = planetValue["orbitAngle"];
                if(orbit.hasKey("x")) planet.orbitAngle.x = orbit["x"].asFloat();
                if(orbit.hasKey("y")) planet.orbitAngle.y = orbit["y"].asFloat();
                if(orbit.hasKey("z")) planet.orbitAngle.z = orbit["z"].asFloat();
            }

            currentPreset.planets.push_back(planet);
        }

        return validatePreset();
    } catch(const std::exception& err) {
        std::cerr << "Error parsing planet: " << err.what() << std::endl;
        return false;
    }
}

/*
** Load Preset
*/
bool PresetLoader::loadPreset(const std::string& path) {
    std::ifstream file(path);
    if(!file.is_open()) {
        std::cerr << "Failed to open preset file: " << path << std::endl;
        return false;
    }

    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string dataStr = buffer.str();
        return parse(dataStr);
    } catch(const std::exception& err) {
        std::cerr << "Error loading preset" << err.what() << std::endl;
        return false;
    }
}

bool PresetLoader::loadDefaultPreset() {
    return loadPreset(defaultPresetPath);
}

/*
** Validate Preset
*/
bool PresetLoader::validatePreset() {
    if(currentPreset.planets.empty()) {
        std::cerr << "No planets in preset!" << std::endl;
        return false;
    }

    bool hasCenter = false;
    for(const auto& planet : currentPreset.planets) {
        if(planet.position == 0) {
            hasCenter = true;
            break;
        }
    }
    if(!hasCenter) {
        std::cerr << "No center planet found!" << std::endl;
        return false;
    }

    std::vector<int> positions;
    for(const auto& planet : currentPreset.planets) {
        if(planet.position != 0) {
            if(std::find(
                positions.begin(),
                positions.end(),
                planet.position
            ) != positions.end()) {
                std::cerr << "Duplicate position found: " << planet.position << std::endl;
                return false;
            }
            positions.push_back(planet.position);
        }
    }

    return true;
}

PresetData& PresetLoader::getCurrentPreset() {
    return currentPreset;
}
PresetData PresetLoader::getCurrentPreset() const {
    return currentPreset;
}

void PresetLoader::setCurrentPreset(const PresetData& preset) {
    currentPreset = preset;
}