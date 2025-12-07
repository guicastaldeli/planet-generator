#include "default_data.h"
#include "../.preset/preset_manager.h"
#include <fstream>
#include <sstream>
#include <emscripten.h>

DefaultData::DefaultData(PresetManager* presetManager) :
    presetManager(presetManager)
{};
DefaultData::~DefaultData() {};

void DefaultData::init() {
    setPath();
    setData();
}

void DefaultData::setPath() {
    path = "/_data/default_planet_data.json";
    presetManager->getPresetLoader()->setPath(path);
}

bool DefaultData::setData() {
    std::ifstream file(path);
    if(!file.is_open()) {
        std::cerr << "Failed to open default data file: " << path << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string data = buffer.str();    
    bool res = presetManager->getPresetLoader()->parse(data);
    
    if(res) {
        const auto& planets = presetManager->getPresetLoader()->getCurrentPreset().planets;
        defaultData.clear();
        
        for(const auto& planet : planets) {
            DefaultData::Data data;
            data.id = planet.id;
            data.name = planet.name;
            data.shape = planet.shape;
            data.size = planet.size;
            data.color = planet.color;
            data.position = planet.position;
            data.rotationDir = planet.rotationDir;
            data.rotationSpeedItself = planet.rotationSpeedItself;
            data.rotationSpeedCenter = planet.rotationSpeedCenter;
            data.distanceFromCenter = planet.distanceFromCenter;
            data.currentRotation = planet.currentRotation;
            data.orbitAngle = planet.orbitAngle;
            
            defaultData.push_back(data);
        }
    } else {
        std::cerr << "Failed to parse default data!" << std::endl;
    }
    
    return res;
}

PlanetData DefaultData::Data::toPlanetData() const {
    PlanetData data;

    data.id = this->id;
    data.name = this->name;
    data.shape = this->shape;
    data.size = this->size;
    data.color = this->color;
    data.position = this->position;
    data.rotationDir = this->rotationDir;
    data.rotationSpeedItself = this->rotationSpeedItself;
    data.rotationSpeedCenter = this->rotationSpeedCenter;
    data.distanceFromCenter = this->distanceFromCenter;
    data.currentRotation = this->currentRotation;
    data.orbitAngle = this->orbitAngle;

    return data;
}

const std::vector<DefaultData::Data>& DefaultData::getAllData() const {
    return defaultData;
}