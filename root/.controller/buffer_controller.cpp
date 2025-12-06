#include "buffer_controller.h"
#include "../.buffers/buffers.h"
#include "preview_controller.h"
#include <iostream>

BufferController::BufferController(
    Main* main,
    Camera* camera, 
    ShaderLoader* shaderLoader
) :
    main(main),
    camera(camera),
    shaderLoader(shaderLoader),
    bufferGenerator(nullptr),
    presetManager(nullptr),
    buffers(nullptr),
    raycaster(nullptr),
    selectedPlanetIndex(-1),
    presetLoaded(false)
{};
BufferController::~BufferController() {};

/*
** Init
*/
void BufferController::initBuffers() {
    buffers = new Buffers(camera, shaderLoader->shaderController, this);
    buffers->init();
}

void BufferController::initPresetManager() {
    presetManager = new PresetManager(this);
}

void BufferController::initGenerator() {
    bufferGenerator = new BufferGenerator(camera);
}

void BufferController::initPreviewController() {
    previewController = new PreviewController(this, camera);
}

void BufferController::setPresetPath() {
    std::string path = "/_data/default-preset.json";
    presetManager->getPresetLoader()->setPath(path);
}

void BufferController::init() {
    initPresetManager();
    setPresetPath();
    initGenerator();
    initPreviewController();
}

/*
** Update Planet Positions
*/
void BufferController::updatePlanetPositions() {
    for(auto& planet : buffers->planetBuffers) {
        float orbitRadius = planet.data.distanceFromCenter;
        float orbitAngle = planet.data.orbitAngle.y;
        planet.worldPos = glm::vec3(
            orbitRadius * cos(glm::radians(orbitAngle)),
            0.0f,
            orbitRadius * sin(glm::radians(orbitAngle))
        );
    }
}

/*
** Check Planet Intersections
*/
int BufferController::checkPlanetIntersections(double mouseX, double mouseY) {
    if(!raycaster || buffers->planetBuffers.empty()) {
        selectedPlanetIndex = -1;
        return -1;
    }
    selectedPlanetIndex = -1;

    for(int i = 0; i < buffers->planetBuffers.size(); i++) {
        auto& planet = buffers->planetBuffers[i];
        if(raycaster->checkIntersection(
            mouseX, mouseY,
            main->width, main->height,
            planet.worldPos,
            planet.data.size,
            i
        )) {
            selectedPlanetIndex = i;
            break;
        }
    }

    return selectedPlanetIndex;
}

/*
** Handle Raycaster
*/
void BufferController::handleRaycasterRender(double mouseX, double mouseY) {
    if(!raycaster) return;

    int hoveredPlanetIndex = checkPlanetIntersections(mouseX, mouseY);
    if(hoveredPlanetIndex != -1) {
        auto& planet = buffers->planetBuffers[hoveredPlanetIndex];
        raycaster->render(
            mouseX,
            mouseY,
            planet.worldPos,
            planet.data.size,
            hoveredPlanetIndex
        );
    } else {
        raycaster->setIsIntersecting(false);
    }
}

void BufferController::handleRaycasterClick(double mouseX, double mouseY) {
    int clickedPlanetIndex = checkPlanetIntersections(mouseX, mouseY);
    if(clickedPlanetIndex != -1) {
        auto& planet = buffers->planetBuffers[clickedPlanetIndex];
        if(raycaster->handleClick(
            mouseX, mouseY,
            main->width, main->height,
            planet.worldPos,
            planet.data.size,
            clickedPlanetIndex
        )) {
            camera->zoomToObj(planet.worldPos, planet.data.size);
        }
    }
}

/*
** Get Selected Planet
*/
const PlanetBuffer* BufferController::getSelectedPlanet() const {
    if(
        camera && 
        camera->isFollowingPlanet &&
        camera->followingPlanetIndex != -1
    ) {
        if(camera->followingPlanetIndex < buffers->planetBuffers.size()) {
            return &buffers->planetBuffers[camera->followingPlanetIndex];
        }
    }
    if(selectedPlanetIndex >= 0 && 
        selectedPlanetIndex < buffers->planetBuffers.size()
    ) {
        return &buffers->planetBuffers[selectedPlanetIndex];
    }

    return nullptr;
}

int BufferController::getSelectedPlanetIndex() const {
    return selectedPlanetIndex;
}

void BufferController::setCamera(Camera* cam) {
    camera = cam;
    if(camera && buffers && shaderLoader) {
        if (raycaster) {
            delete raycaster;
            raycaster = nullptr;
        }
        raycaster = new Raycaster(
            main,
            camera,
            buffers,
            shaderLoader->shaderController
        );
    }
}

void BufferController::clearBuffers() {
    if(buffers) buffers->clearBuffers();
    if(presetManager->getPresetLoader()) {
        presetManager->getPresetLoader()->getCurrentPreset().planets.clear();
        currentPreset = presetManager->getPresetLoader()->getCurrentPreset();
    }

    selectedPlanetIndex = -1;
    if(raycaster) {
        raycaster->setIsIntersecting(false);
        raycaster->selectedPlanetIndex = -1;
    }
}

void BufferController::deleteSelectedPlanet() {
    if(selectedPlanetIndex == -1 || buffers->planetBuffers.empty()) {
        std::cout << "No planet to delete" << std::endl;
        return;
    }

    if(selectedPlanetIndex < buffers->planetBuffers.size()) {
        buffers->
            planetBuffers.erase(
                buffers->planetBuffers.begin() + 
                selectedPlanetIndex
            );
    }
    if(
        presetManager->getPresetLoader() && 
        selectedPlanetIndex < presetManager->getPresetLoader()->getCurrentPreset().planets.size()
    ) {
        presetManager->getPresetLoader()->getCurrentPreset().planets.erase(
            presetManager->getPresetLoader()->getCurrentPreset().planets.begin() + selectedPlanetIndex
        );
    }
    if(
        camera &&
        camera->isFollowingPlanet &&
        camera->followingPlanetIndex == selectedPlanetIndex
    ) {
        camera->resetToSavedPos();
    }

    selectedPlanetIndex = -1;
    if(raycaster) {
        raycaster->selectedPlanetIndex = -1;
        raycaster->setIsIntersecting(false);
    }
}

/*
** Render
*/
void BufferController::render(float deltaTime) {
    if(!presetLoaded) {
        if(presetManager->getPresetLoader()->loadDefaultPreset()) {
            currentPreset = presetManager->getPresetLoader()->getCurrentPreset();
            presetLoaded = true;

            std::vector<PlanetBuffer> newPlanetBuffers = bufferGenerator->generateFromPreset(currentPreset);
            buffers->planetBuffers.clear();
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
                buffers->createBufferForPlanet(planetBuffer);
                buffers->planetBuffers.push_back(std::move(planetBuffer));
            }
        } else {
            printf("ERR failed to load preset!\n");
            return;
        }
    }
    bufferGenerator->updatePlanetRotation(buffers->planetBuffers, deltaTime);
    updatePlanetPositions();
    buffers->render();
}