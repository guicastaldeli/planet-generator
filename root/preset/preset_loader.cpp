#include "preset_loader.h"
#include "preset_data.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

PresetLoader::PresetLoader(const std::string& path) :
    defaultPresetPath(path) 
{};
PresetLoader::~PresetLoader() {};

/*
** Load Preset
*/
bool PresetLoader::loadPreset(const std::string& path) {
    std::ifstream file(path);
    if(!file.is_open()) {
        std::cerr << "Failed to open preset file" << path << std::endl;
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

PresetData PresetLoader::getCurrentPreset() const {
    return currentPreset;
}

void PresetLoader::setCurrentPreset(const PresetData& preset) {
    currentPreset = preset;
}