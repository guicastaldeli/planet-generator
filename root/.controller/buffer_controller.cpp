#include "buffer_controller.h"
#include "../.buffers/buffers.h"

BufferController::BufferController(
    Main* main,
    Camera* camera, 
    ShaderLoader* shaderLoader
) :
    main(main),
    camera(camera),
    shaderLoader(shaderLoader),
    bufferGenerator(nullptr),
    presetLoader(nullptr),
    buffers(nullptr),
    raycaster(nullptr),
    selectedPlanetIndex(-1)
{};
BufferController::~BufferController() {};

/*
** Init
*/
void BufferController::initBuffers() {
    buffers = new Buffers(camera, shaderLoader->shaderController, this);
    buffers->init();
}

void BufferController::initPresetLoader() {
    std::string path = "/_data/default-preset.json";
    presetLoader = new PresetLoader(path);
}

void BufferController::initGenerator() {
    bufferGenerator = new BufferGenerator();
}

void BufferController::init() {
    initBuffers();
    initPresetLoader();
    initGenerator();
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
    if(selectedPlanetIndex >= 0 && selectedPlanetIndex < buffers->planetBuffers.size()) {
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

/*
** Render
*/
// ...existing code...
/*
** Render
*/
void BufferController::render(float deltaTime) {
    static bool presetLoaded = false;

    if(!presetLoaded) {
        if(presetLoader->loadDefaultPreset()) {
            currentPreset = presetLoader->getCurrentPreset();
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
                buffers->createBufferForPlanet(planetBuffer);
                buffers->planetBuffers.push_back(std::move(planetBuffer));
            }
        } else {
            printf("ERR failed to load preset!\n");
            return;
        }
    }
    
    if(!buffers->planetBuffers.empty()) {
        bufferGenerator->updatePlanetRotation(buffers->planetBuffers, deltaTime);
        updatePlanetPositions();
        buffers->render();
    } else {
        printf("ERR: no planets to render!");
    }
}