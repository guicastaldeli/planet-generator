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
void BufferController::init() {
    initPresetManager();
    setDefaultData();
    setPresetPath();
    initGenerator();
    initPreviewController();

    if(presetManager && presetManager->getPresetImporter()) {
        presetManager->getPresetImporter()->setImportCallback(
            [this](const PresetData& preset) {
                this->onPresetImported(preset);
            }
        );
    }
}

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

void BufferController::setDefaultData() {
    defaultData = new DefaultData(presetManager);
    defaultData->init();
}

/*
** Preset
*/
void BufferController::setPresetPath() {
    std::string path = "/_data/default_preset.json";
    presetManager->getPresetLoader()->setPath(path);
}

PresetData BufferController::getCurrentPreset() const {
    return currentPreset;
}

void BufferController::onPresetImported(const PresetData& preset) {
    PresetData presetCopy = preset;
    loadPresetData(presetCopy);
}

/*
** Set Data to Update
*/
void BufferController::setDataToUpdate(PlanetData uData, auto pData) {
    DefaultData::Data* dData;

    uData.id = dData->id;
    
    uData.name = 
        pData.hasKey("name") ?
        pData["name"].asString() :
        dData->name;

    if(pData.hasKey("shape")) {
        std::string shape = pData["shape"].asString();
        uData.shape = bufferGenerator->shapeToBufferType(shape);
    } else {
        uData.shape = dData->shape
    }
            
    uData.size = 
        pData.hasKey("size") ? 
        pData["size"].asFloat() : 
        dData->size;
            
    uData.color =
        pData.hasKey("color") ?
        pData["color"].asString() :
        dData->color;

    if(pData.hasKey("rotationDir")) {
        std::string rotation = pData["rotationDir"].asString();
        uData.rotationDir = bufferGenerator->rotationToBufferType(rotation);
    } else {
        uData.rotationDir = dData->rotationDir;
    }

    uData.rotationSpeedItself = 
        pData.hasKey("rotationSpeedItself") ?
        pData["rotationSpeedItself"].asFloat() :
        dData->rotationSpeedItself;
            
    uData.rotationSpeedCenter =
        pData.hasKey("rotationSpeedCenter") ?
        pData["rotationSpeedCenter"].asFloat() :
        dData->rotationSpeedCenter;
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
            i,
            planet.data.shape
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
            hoveredPlanetIndex,
            planet.data.shape
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
            clickedPlanetIndex,
            planet.data.shape
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
        return;
    }

    if(selectedPlanetIndex < buffers->planetBuffers.size()) {
        std::string planetName = buffers->planetBuffers[selectedPlanetIndex].data.name;
        buffers->planetBuffers.erase(
            buffers->planetBuffers.begin() + selectedPlanetIndex
        );
    }
    if(selectedPlanetIndex < currentPreset.planets.size()) {
        std::string planetName = currentPreset.planets[selectedPlanetIndex].name;
        currentPreset.planets.erase(
            currentPreset.planets.begin() + selectedPlanetIndex
        );
    }
    if(presetManager->getPresetLoader() && 
       selectedPlanetIndex < presetManager->getPresetLoader()->getCurrentPreset().planets.size()) {
        presetManager->getPresetLoader()->getCurrentPreset().planets.erase(
            presetManager->getPresetLoader()->getCurrentPreset().planets.begin() + selectedPlanetIndex
        );
    }
    if(camera && camera->isFollowingPlanet && camera->followingPlanetIndex == selectedPlanetIndex) {
        camera->resetToSavedPos();
    }
    selectedPlanetIndex = -1;
    if(raycaster) {
        raycaster->selectedPlanetIndex = -1;
        raycaster->setIsIntersecting(false);
    }
}

bool BufferController::isPreviewActive() const {
    return previewController &&
        (previewController->isPreviewing ||
        previewController->isGeneratorActive);
}

/*
** Load Preset Data
*/
void BufferController::loadPresetData(PresetData& preset) {
    currentPreset = preset;
    presetLoaded = true;
    if(presetManager && presetManager->getPresetLoader()) {
        presetManager->getPresetLoader()->setCurrentPreset(preset);
    }
    if(buffers) {
        buffers->planetBuffers.clear();
        buffers->clearBuffers();
    }

    std::vector<PlanetBuffer> newPlanetBuffers = bufferGenerator->generateFromPreset(currentPreset);
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