#include "preset_saver.h"
#include "../.controller/buffer_controller.h"
#include "preset_manager.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <iostream>
#include <sstream>

PresetSaver::PresetSaver(BufferController* bufferController, PresetManager* presetManager) :
    bufferController(bufferController),
    presetManager(presetManager),
    key("savedPreset")
{};
PresetSaver::~PresetSaver() {};

/*
** Planet To Value
*/
DataParser::Value PresetSaver::planetToValue(const PlanetData& data) {
    using namespace DataParser;
    
    std::string shapeStr = bufferController->bufferGenerator->shapeToString(data.shape);
    std::string rotationStr = bufferController->bufferGenerator->rotationToString(data.rotationDir);
    
    Value result(ValueType::Object);
    result["id"] = Value(static_cast<double>(data.id));
    result["name"] = Value(data.name);
    result["size"] = Value(data.size);
    result["color"] = Value(data.color);
    result["position"] = Value(static_cast<double>(data.position));
    result["distanceFromCenter"] = Value(data.distanceFromCenter);
    result["rotationSpeedItself"] = Value(data.rotationSpeedItself);
    result["rotationSpeedCenter"] = Value(data.rotationSpeedCenter);
    result["shape"] = Value(shapeStr);
    result["rotationDir"] = Value(rotationStr);
    
    Value currentRotation(ValueType::Object);
    currentRotation["x"] = Value(data.currentRotation.x);
    currentRotation["y"] = Value(data.currentRotation.y);
    currentRotation["z"] = Value(data.currentRotation.z);
    result["currentRotation"] = currentRotation;
    
    Value orbitAngle(ValueType::Object);
    orbitAngle["x"] = Value(data.orbitAngle.x);
    orbitAngle["y"] = Value(data.orbitAngle.y);
    orbitAngle["z"] = Value(data.orbitAngle.z);
    result["orbitAngle"] = orbitAngle;
    
    return result;
}

bool PresetSaver::valueToPlanet(const DataParser::Value& value, PlanetData& data) {
    using namespace DataParser;

    try {
        data.id = value["id"].asInt();
        data.name = value["name"].asString();
        data.size = value["size"].asFloat();
        data.color = value["color"].asString();
        data.position = value["position"].asInt();
        data.distanceFromCenter = value["distanceFromCenter"].asFloat();
        data.rotationSpeedItself = value["rotationSpeedItself"].asFloat();
        data.rotationSpeedCenter = value["rotationSpeedCenter"].asFloat();

        std::string shapeStr = value["shape"].asString();
        if(shapeStr == "SPHERE") {
            data.shape = BufferData::Type::SPHERE;
        } else if(shapeStr == "CUBE") {
            data.shape = BufferData::Type::CUBE;
        } else if(shapeStr == "TRIANGLE") {
            data.shape = BufferData::Type::TRIANGLE;
        } else {
            data.shape = BufferData::Type::SPHERE;
        }

        std::string rotationStr = value["rotationDir"].asString();
        if(rotationStr == "X") {
            data.rotationDir = RotationAxis::X;
        } else if(rotationStr == "Y") {
            data.rotationDir = RotationAxis::Y;
        } else if(rotationStr == "Z") {
            data.rotationDir = RotationAxis::Z;
        } else {
            data.rotationDir = RotationAxis::Y;
        }

        if(value.hasKey("currentRotation")) {
            const Value& rot = value["currentRotation"];
            data.currentRotation.x = rot["x"].asFloat();
            data.currentRotation.y = rot["y"].asFloat();
            data.currentRotation.z = rot["z"].asFloat();
        }
        
        if(value.hasKey("orbitAngle")) {
            const Value& orbit = value["orbitAngle"];
            data.orbitAngle.x = orbit["x"].asFloat();
            data.orbitAngle.y = orbit["y"].asFloat();
            data.orbitAngle.z = orbit["z"].asFloat();
        }

        return true;
    } catch(const std::exception& err) {
        std::cerr << "Error converting value: " << err.what() << std::endl;
        return false;
    }
}

/*
** Preset To Value
*/
DataParser::Value PresetSaver::presetToVal(PresetData& preset) {
    using namespace DataParser;

    Value result(ValueType::Object);
    Value planetsArray(ValueType::Array);
    for(const auto& planet : preset.planets) {
        planetsArray.push_back(planetToValue(planet));
    }
    
    result["planets"] = planetsArray;
    return result;
}

/*
** Value to Preset
*/
bool PresetSaver::valueToPreset(const DataParser::Value& value, PresetData& preset) {
    using namespace DataParser;

    try {
        if(!value.hasKey("planets") || !value["planets"].isArray()) {
            std::cerr << "Invalid preset format: missing planets array" << std::endl;
            return false;
        }

        const Value& planetsArray = value["planets"];
        preset.planets.clear();
        for(size_t i = 0; i < planetsArray.size(); i++) {
            PlanetData planet;
            if(valueToPlanet(planetsArray[i], planet)) {
                preset.planets.push_back(planet);
            } else {
                std::cerr << "Failed to parse planet at index: " << i << std:: endl;
                return false;
            }
        }

        return true;
    } catch(const std::exception& err) {
        std::cerr << "Error converting value to preset: " << err.what() << std::endl;
        return false;
    }
}

std::string PresetSaver::presetToJson(PresetData& preset) {
    DataParser::Value presetValue = presetToVal(preset);
    return presetValue.toString(false);
}

bool PresetSaver::jsonToPreset(const std::string& data, PresetData& preset) {
    try {
        DataParser::Value parsedValue = DataParser::Parser::parse(data);
        return valueToPreset(parsedValue, preset);
    } catch(const std::exception& err) {
        std::cerr << "Error parsing JSON: " << err.what() << std::endl;
        return false;
    }
}

/*
** Save to Local Storage
*/
bool PresetSaver::saveToLocalStorage(PresetData& preset) {
    std::string jsonData = presetToJson(preset);
    bool success = EM_ASM_INT({
        try {
            const key = UTF8ToString($0);
            const dataStr = UTF8ToString($1);
            localStorage.setItem(key, dataStr);
            console.log('Preset saved to localStorage with key:', key);
            console.log('JSON saved:', dataStr);
            return 1;
        } catch(err) {
            console.error('Error saving to local storage', err);
            return 0;
        }
    }, key.c_str(), jsonData.c_str());
    return success == 1;
}

/*
** Load from Local Storage
*/
bool PresetSaver::loadFromLocalStorage(PresetData& preset) {
    char* str = (char*)EM_ASM_INT({
        try {
            const key = UTF8ToString($0);
            const data = localStorage.getItem(key);
            if(data) {
                console.log('Found preset in localstorage with key: ', key);
                var lengthBytes = lengthBytesUTF8(data) + 1;
                var strPtr = _malloc(lengthBytes);
                stringToUTF8(data, strPtr, lengthBytes);
                return strPtr;
            } else {
                console.log('No preset found in local storage!: ', key);
                return 0;
            }
        } catch(err) {
            console.error('Error loading!', err);
            return 0.
        }
    }, key.c_str());

    if(str == 0) return false;
    std::string dataJson(str);
    free(str);
    return jsonToPreset(dataJson, preset);
}

bool PresetSaver::hasSavedPreset() {
    return EM_ASM_INT({
        try {
            const key = UTF8ToString($0);
            return localStorage.getItem(key) !== null ? 1 : 0;
        } catch(err) {
            console.error('Error checking localStorage', err);
            return 0;
        }
    }, key.c_str()) == 1;
}

void PresetSaver::clearLocalStorage() {
    EM_ASM({
        try {
            const key = UTF8ToString($0);
            localStorage.removeItem(key);
            console.log('Removed preset from localStorage with key:', key);
        } catch(err) {
            console.error('Error clearing localStorage:', err);
        }
    }, key.c_str());
}

/*
**
*** Save
**
*/
bool PresetSaver::save() {
    if(!bufferController) {
        std::cerr << "No bufferController!, ERR. **save" << std::endl;
        return false;
    }

    PresetData currentPreset = bufferController->getCurrentPreset();
    if (currentPreset.planets.empty()) {
        std::cerr << "Cannot save: preset has no planets" << std::endl;
        return false;
    }

    bool success = saveToLocalStorage(bufferController->currentPreset);
    if (success) {
        std::cout << "Preset saved to localStorage successfully!" << std::endl;
    } else {
        std::cerr << "Failed to save preset to localStorage" << std::endl;
    }

    return success;
}

/*
**
*** Save
**
*/
bool PresetSaver::load() {
     if (!bufferController) {
        std::cerr << "No bufferController!, ERR. **load" << std::endl;
        return false;
    }

    PresetData loadedPreset;
    if(loadFromLocalStorage(loadedPreset)) {
        presetManager->getPresetLoader()->setCurrentPreset(loadedPreset);
        bufferController->loadPresetData(loadedPreset);
        return true;
    }

    return false;
}