#include "preset_reset.h"
#include "../.controller/buffer_controller.h"
#include "../.buffers/buffers.h"
#include <iostream>

PresetReset::PresetReset(BufferController* bufferController) :
    bufferController(bufferController) 
{};
PresetReset::~PresetReset() {};

void PresetReset::resetToDefault() {
    if(!bufferController->presetManager->getPresetLoader()) {
        std::cout << "No preset loader available!" << std::endl;
        return;
    }

    bufferController->clearBuffers();
    if(
        bufferController->
            presetManager->
            getPresetLoader()-> 
            loadDefaultPreset()
        ) {
        bufferController->currentPreset = bufferController->
            presetManager->
            getPresetLoader()-> 
            getCurrentPreset();
        bufferController->presetLoaded = true;

        std::vector<PlanetBuffer> newPlanetBuffers = bufferController->
            bufferGenerator->
            generateFromPreset(bufferController->currentPreset);

        bufferController->planetBuffers.clear();
        for(auto& planetBuffer : newPlanetBuffers) {
            float orbitRadius = planetBuffer.data.distanceFromCenter;
            float initialAngle = planetBuffer.data.orbitAngle.y;

            glm::vec3 planetPosition(
                orbitRadius * cos(glm::radians(initialAngle)),
                0.0f,
                orbitRadius * sin(glm::radians(initialAngle))
            );
            planetBuffer.worldPos = planetPosition;
            planetBuffer.isPreview = false;
            bufferController->buffers->createBufferForPlanet(planetBuffer);
            bufferController->buffers->planetBuffers.push_back(std::move(planetBuffer));
        }
        std::cout << "Reseted to default!" << std::endl;
    } else {
        std::cout << "Failed to reset!" << std::endl;
    }

    if(bufferController->camera) {
        bufferController->camera->isFollowingPlanet = false;
        bufferController->camera->followingPlanetIndex = -1;
        bufferController->camera->resetToSavedPos();
    }
    if(bufferController->raycaster) {
        bufferController->raycaster->setIsIntersecting(false);
        bufferController->raycaster->selectedPlanetIndex = -1;
    }
    bufferController->selectedPlanetIndex = -1;
}