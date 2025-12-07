#include "default_data.h"
#include "../.preset/preset_manager.h"

DefaultData::DefaultData(PresetManager* presetManager) :
    presetManager(presetManager),
    path(nullptr)
{};
DefaultData::~DefaultData() {};

void DefaultData::init() {
    setPath();
}

void DefaultData::setPath() {
    path = "/_data/default_planet_data.json";
    presetManager->getPresetLoader()->setPath(path);
}

bool DefaultData::setData() {
    bool data = presetManager->getPresetLoader()->parse(path);
    return data;
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



