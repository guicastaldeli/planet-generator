#include "preview_controller.h"
#include "../buffers/buffers.h"
#include "../_utils/default_data.h"
#include <emscripten.h>

PreviewController::PreviewController(
    BufferController* bufferController, 
    Camera* camera,
    LightManager* lightManager
) :
    bufferController(bufferController),
    camera(camera),
    lightManager(lightManager),
    isPreviewing(false),
    isGeneratorActive(false),
    previewLightId(PREVIEW_LIGHT_ID)
{};
PreviewController::~PreviewController() {};

/**
 * 
 * Lock and Unlock Camera
 * 
 */
void PreviewController::lockCamera() {
    if(camera) {
        camera->lockPanning(true);
        camera->lockRotation(true);
        camera->lockZoom(true);
    }
}

void PreviewController::unlockCamera() {
    if(camera) {
        camera->lockPanning(false);
        camera->lockRotation(false);
        camera->lockZoom(false);
    }
}

/**
 * Preview
 */
void PreviewController::preview() {
    isGeneratorActive = true;
    isPreviewing = true;
    if(isPreviewing && isGeneratorActive) {
        camera->zoomLevel = 45.0f;
        camera->updateProjection();
        lockCamera();
    }
}

void PreviewController::exitPreview() {
    isGeneratorActive = false;
    isPreviewing = false;
    if(!isPreviewing && !isGeneratorActive) {
        unlockCamera();
        camera->releaseCamera();
    }
}

/**
 * Start Generator Preview
 */
void PreviewController::startGeneratorPreview() {
    isGeneratorActive = true;
    isPreviewing = true;

    if(camera) {
        emscripten_console_log("camera");
        camera->saveCurrentPos();
        preview();
    } else {
        emscripten_console_log("no camera");
        preview();
    }

    lightManager->getPointLight()->hide();

    PlanetData previewData;
    if(bufferController && 
        bufferController->defaultData &&
        !bufferController->defaultData->getAllData().empty()
    ) {
        const DefaultData::Data* defaultDataPtr = &bufferController->defaultData->getAllData()[0];
        previewData.id = defaultDataPtr->id;
        previewData.name = defaultDataPtr->name;
        previewData.shape = defaultDataPtr->shape;
        previewData.size = defaultDataPtr->size;
        previewData.color = defaultDataPtr->color;
        previewData.colorRgb = defaultDataPtr->colorRgb;
        previewData.texture = defaultDataPtr->texture;
        previewData.position = defaultDataPtr->position;
        previewData.lightning = defaultDataPtr->lightning;
        previewData.hasSunLight = defaultDataPtr->hasSunLight;
        previewData.effects = defaultDataPtr->effects;
        previewData.effectType = defaultDataPtr->effectType;
        previewData.rotationDir = defaultDataPtr->rotationDir;
        previewData.rotationSpeedItself = defaultDataPtr->rotationSpeedItself;
        previewData.rotationSpeedCenter = defaultDataPtr->rotationSpeedCenter;
        previewData.distanceFromCenter = defaultDataPtr->distanceFromCenter;
        previewData.currentRotation = defaultDataPtr->currentRotation;
        previewData.orbitAngle = defaultDataPtr->orbitAngle;
    }
    
    if(bufferController && bufferController->buffers) {
        bufferController->buffers->setupPreviewPlanet(previewData);
        bufferController->buffers->setPreviewMode(true);
    }

    createPreviewLight();
}

/**
 * Update Preview
 */
void PreviewController::updatePreview(const PlanetData& data) {
    if(!isGeneratorActive) return;
    if(!bufferController || !bufferController->buffers) return;

    PlanetData previewData = data;
    previewData.id = -1;
    PlanetBuffer planetBuffer;
    planetBuffer.isPreview = true;
    bufferController->buffers->updatePreviewPlanet(previewData);
}

/**
 * Cleanup Preview
 */
void PreviewController::cleanupPreview() {
    if(!isPreviewing && !isGeneratorActive) return;

    if(camera) {
        camera->releaseCamera();
        unlockCamera();
    }
    if(isPreviewing) {
        exitPreview();
    }
    if(bufferController && bufferController->buffers) {
        bufferController->buffers->cleanupPreviewPlanet();
        bufferController->buffers->setPreviewMode(false);
    }

    removePreviewLight();
    lightManager->getPointLight()->show();

    isGeneratorActive = false;
    isPreviewing = false;

    emscripten_log(EM_LOG_CONSOLE, "Cleanup completed!");
}

/**
 * Clear Texture
 */
void PreviewController::clearCurrentTexture() {
    if(!isGeneratorActive) return;

    PlanetData currentData = getCurrentPreviewData();
    currentData.texture = "";
    if(bufferController && bufferController->buffers) {
        bufferController->buffers->updatePreviewPlanet(currentData);
    }

    currentPreviewData = currentData;
}

/**
 * Get Current Preview Data
 */
PlanetData PreviewController::getCurrentPreviewData() const {
    PlanetData data;
    if(bufferController && bufferController->buffers) {
        const auto& previewBuffer = bufferController->buffers->getPreviewBuffer();
        if(!previewBuffer.data.name.empty()) {
            data = previewBuffer.data;
        } else {
            data = currentPreviewData;
        }
    } else {
        data = currentPreviewData;
    }

    return data;
}

/**
 * Preview Light
 */
void PreviewController::createPreviewLight() {
    if(isPreviewing && isGeneratorActive) {
        PointLight previewLight;
        previewLight.position = glm::vec3(2.0f, 0.0f, 5.0f);
        previewLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
        previewLight.intensity = 1.5f;
        previewLight.constant = 1.0f;
        previewLight.linear = 0.09f;
        previewLight.quadratic = 0.032f;
        previewLight.associatedPlanetId = previewLightId; 
        previewLight.planetName = "PreviewLight";
        previewLight.isSunLight = false;
        previewLight.isHidden = false;  
        previewLight.calcRadius();
        
        lightManager->getPointLight()->add(previewLight);
        
        emscripten_log(EM_LOG_CONSOLE, "Preview light created");
    }
}

void PreviewController::removePreviewLight() {
    bool removeAny = false;
    auto& lights = lightManager->getPointLight()->pointLights;
    for(auto it = lights.begin(); it != lights.end();) {
        if(it->associatedPlanetId == previewLightId) {
            it = lights.erase(it);
            removeAny = true;
        } else {
            ++it;
        }
    }
}

bool PreviewController::isInGeneratorMode() const { 
    return isGeneratorActive;
}