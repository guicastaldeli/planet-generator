#include "preset_loader.h"
#include "preset_data.h"
#include "../_data/data_parser.h"
#include "../_utils/color_converter.h"
#include "../lightning/light_manager.h"
#include "preset_manager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

PresetLoader::PresetLoader(PresetManager* presetManager) :
    presetManager(presetManager) 
{};
PresetLoader::~PresetLoader() {};

/**
 * Set Path
 */
void PresetLoader::setPath(std::string& path) {
    defaultPresetPath = path;
}

/**
 * Load Preset
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
    if(presetManager->getPresetSaver()->hasSavedPreset()) {
        PresetData localStorageData;
        if(presetManager->getPresetSaver()->loadFromLocalStorage(localStorageData)) {
            currentPreset = localStorageData;
            return true;
        } else {
            std::cout << "Failed to load from localStorage, falling back to file" << std::endl;
        }
    } else {
        std::cout << "No saved preset in localStorage" << std::endl;
    }

    std::cout << "Loading default preset from file: " << defaultPresetPath << std::endl;
    return loadPreset(defaultPresetPath);
}


bool PresetLoader::loadDefaultPresetFile() {
    std::string path = "/_data/default_preset.json";
    return loadPreset(path);
}

/**
 * Validate Preset
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

/**
 * Current Preset
 */
void PresetLoader::setCurrentPreset(const PresetData& preset) {
    currentPreset = preset;
}

PresetData& PresetLoader::getCurrentPreset() {
    return currentPreset;
}
PresetData PresetLoader::getCurrentPreset() const {
    return currentPreset;
}

/**
 * Init Lights
 */
void PresetLoader::initLightsFromPreset(LightManager* lightManager) {
    if(!lightManager) return;

    for(const auto& planet : currentPreset.planets) {
        if(planet.hasSunLight) {
            PointLight pointLight = planet.sunLight;
            pointLight.position = glm::vec3(0.0);
            if(planet.position == 0) pointLight.position = glm::vec3(0.0);

            pointLight.associatedPlanetId = planet.id;
            lightManager->getPointLight()->add(pointLight);
        }
    }
}

/**
 * 
 * Parse
 * 
 */
bool PresetLoader::parse(const std::string& data) {
    try {
        DataParser::Value root = DataParser::Parser::parse(data);
        currentPreset.planets.clear();

        if(root.hasKey("planets") && root["planets"].isArray()) {
            const auto& dataArray = root["planets"].asArray();
            for(const auto& val : dataArray) {
                if(!val.isObject()) continue;
                PlanetData data;
                parseData(val, data);
                currentPreset.planets.push_back(data);
            }
        }
        else if(root.hasKey("name") || root.hasKey("size") || root.hasKey("shape")) {
            PlanetData data;
            parseData(root, data);
            currentPreset.planets.push_back(data);
        }
        else {
            std::cerr << "Invalid format! Expected either a planets array or a single planet object." << std::endl;
            return false;
        }

        return validatePreset();
    } catch(const std::exception& err) {
        std::cerr << "Error parsing planet: " << err.what() << std::endl;
        return false;
    }
}

void PresetLoader::parseData(const DataParser::Value& val, PlanetData& data) {
    /* Props */
    if(val.hasKey("id")) data.id = val["id"].asInt();
    if(val.hasKey("name")) data.name = val["name"].asString();
    if(val.hasKey("size")) data.size = val["size"].asFloat();
    if(val.hasKey("color")) {
        data.color = val["color"].asString();
        data.colorRgb = ColorConverter::parseColor(data.color);
    }
    if(val.hasKey("texture")) data.texture = val["texture"].asString();
    if(val.hasKey("position")) data.position = val["position"].asInt();
    if(val.hasKey("distanceFromCenter")) 
        data.distanceFromCenter = val["distanceFromCenter"].asFloat();
    if(val.hasKey("rotationSpeedItself")) 
        data.rotationSpeedItself = val["rotationSpeedItself"].asFloat();
    if(val.hasKey("rotationSpeedCenter")) 
        data.rotationSpeedCenter = val["rotationSpeedCenter"].asFloat();

    /* Shape */
    if(val.hasKey("shape")) {
        std::string shapeStr = val["shape"].asString();
        if(shapeStr == "SPHERE") data.shape = BufferData::Type::SPHERE;
        else if(shapeStr == "CUBE") data.shape = BufferData::Type::CUBE;
        else if(shapeStr == "TRIANGLE") data.shape = BufferData::Type::TRIANGLE;
        else data.shape = BufferData::Type::SPHERE;
    }

    /* Lightning */
    if(val.hasKey("lightning")) {
        std::string lightningStr = val["lightning"].asString();
        data.lightning = lightningStr;
        data.hasSunLight = (lightningStr == "Sun Light");
    } else {
        data.lightning = "None";
        data.hasSunLight = false;
    }

    /* Effects */
    if(val.hasKey("effects")) {
        data.effects = val["effects"].asString();
        if(data.effects == "None") data.effectType = 0;
        else if(data.effects == "Earth Orbit") data.effectType = 1;
        else if(data.effects == "Noise") data.effectType = 2;
        else if(data.effects == "Clouds") data.effectType = 3;
        else if(data.effects == "Rings") data.effectType = 4;
    }

    /* Rotation */
    if(val.hasKey("rotationDir")) {
        std::string rotStr = val["rotationDir"].asString();
        if(rotStr == "X") data.rotationDir = RotationAxis::X;
        else if(rotStr == "Y") data.rotationDir = RotationAxis::Y;
        else if(rotStr == "Z") data.rotationDir = RotationAxis::Z;
        else data.rotationDir = RotationAxis::Y;
    }
    if(val.hasKey("currentRotation") && val["currentRotation"].isObject()) {
        const auto& rot = val["currentRotation"];
        if(rot.hasKey("x")) data.currentRotation.x = rot["x"].asFloat();
        if(rot.hasKey("y")) data.currentRotation.y = rot["y"].asFloat();
        if(rot.hasKey("z")) data.currentRotation.z = rot["z"].asFloat();
    }

    /* Orbit Angle */
    if(val.hasKey("orbitAngle") && val["orbitAngle"].isObject()) {
        const auto& orbit = val["orbitAngle"];
        if(orbit.hasKey("x")) data.orbitAngle.x = orbit["x"].asFloat();
        if(orbit.hasKey("y")) data.orbitAngle.y = orbit["y"].asFloat();
        if(orbit.hasKey("z")) data.orbitAngle.z = orbit["z"].asFloat();
    }
}

std::string PresetLoader::getDefaultPresetPath() const {
    return defaultPresetPath;
}