#include "buffer_controller.h"
#include "buffers.h"

BufferController::BufferController(Camera* Camera, ShaderLoader* shaderLoader) :
    camera(camera),
    shaderLoader(shaderLoader),
    bufferGenerator(nullptr),
    presetLoader(nullptr),
    buffers(nullptr)
{};
BufferController::~BufferController() {};

void BufferController::initBuffers() {
    buffers = new Buffers(
        camera, 
        shaderLoader->shaderController, 
        BufferData::Type::TRIANGLE
    );
    buffers->init();
}

void BufferController::initPresetLoader() {
    presetLoader = new PresetLoader("../.data/default-preset");
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
        planetBuffers = bufferGenerator->generateFromPreset(currentPreset);
        for(const auto& planetBuffer : planetBuffers) {
            BufferData::Type bufferType;
            switch (planetBuffer.data.shape) {
                case BufferData::Type::SPHERE:
                    bufferType = BufferData::Type::SPHERE;
                    break;
                case BufferData::Type::CUBE:
                    bufferType = BufferData::Type::CUBE;
                    break;
                case BufferData::Type::TRIANGLE:
                    bufferType = BufferData::Type::TRIANGLE;
                    break;
            }

            buffers->render();
            bufferGenerator->updatePlanetRotation(planetBuffers, deltaTime);
            buffers->setOrbit();
        }
    }
}