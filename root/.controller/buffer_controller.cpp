#include "buffer_controller.h"
#include "../buffers/buffers.h"

BufferController::BufferController(Camera* camera, ShaderLoader* shaderLoader) :
    camera(camera),
    shaderLoader(shaderLoader),
    bufferGenerator(nullptr),
    presetLoader(nullptr),
    buffers(nullptr)
{};
BufferController::~BufferController() {};

void BufferController::initBuffers() {
    buffers = new Buffers(camera, shaderLoader->shaderController);
    buffers->init();
}

void BufferController::initPresetLoader() {
    std::string path = "/.data/default-preset.json";
    presetLoader = new PresetLoader(path);
}

void BufferController::initGenerator() {
    bufferGenerator = new BufferGenerator();
}

/*
** Init
*/
void BufferController::init() {
    initBuffers();
    initPresetLoader();
    initGenerator();
}

/*
** Render
*/
void BufferController::render(float deltaTime) {
    if(presetLoader->loadDefaultPreset()) {
        currentPreset = presetLoader->getCurrentPreset();
        std::vector<PlanetBuffer> newPlanetBuffers = bufferGenerator->generateFromPreset(currentPreset);
        for(auto& planetBuffer : newPlanetBuffers) {
            buffers->createBufferForPlanet(planetBuffer);
        }

        buffers->planetBuffers.clear();
        for(auto& planetBuffer : newPlanetBuffers) {
            buffers->planetBuffers.push_back(std::move(planetBuffer));
        }

        bufferGenerator->updatePlanetRotation(buffers->planetBuffers, deltaTime);
        buffers->render();
        buffers->setOrbit();
    }
}