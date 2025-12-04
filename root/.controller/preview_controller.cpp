#include "preview_controller.h"
#include "../.buffers/buffers.h"

PreviewController::PreviewController(BufferController* bufferController, Camera* camera) :
    bufferController(bufferController),
    camera(camera),
    isPreviewing(true),
    isGeneratorActive(false)
{};
PreviewController::~PreviewController() {
    cleanupPreview();
};

/*
 *
 * Lock and Unlock Camera
 * 
 */
void PreviewController::lockCamera() {
    if(camera) {
        camera->lockPanning(true);
        camera->lockRotation(true);
    }
}

void PreviewController::unlockCamera() {
    if(camera) {
        camera->lockPanning(false);
        camera->lockRotation(false);
    }
}

/*
** Positionate Camera
*/
void PreviewController::positionateCamera() {
    if(isPreviewing && camera) {
        camera->setPosition(0.0f, 0.0f, 3.0f);
    } else if(camera){
        camera->releaseCamera();
    }
}

/*
** Preview
*/
void PreviewController::preview() {
    isPreviewing = true;
    lockCamera();
    positionateCamera();
}

void PreviewController::exitPreview() {
    isPreviewing = false;
    unlockCamera();
}

/*
** Start Generator Preview
*/
void PreviewController::startGeneratorPreview() {
    if(isGeneratorActive) cleanupPreview();
    isGeneratorActive = true;
    isPreviewing = true;

    PlanetData previewData;
    previewData.id = -1;
    previewData.name = "Preview Planet";
    previewData.shape = BufferData::Type::SPHERE;
    previewData.size = 1.0f;
    previewData.color = "#808080";
    previewData.position = -1;
    previewData.rotationDir = RotationAxis::Y;
    previewData.rotationSpeedItself = 0.01f;
    previewData.rotationSpeedCenter = 0.01f;
    previewData.distanceFromCenter = 0.0f;
    previewData.currentRotation = glm::vec3(0.0f);
    previewData.orbitAngle = glm::vec3(0.0f);

    if(bufferController && bufferController->buffers) {
        bufferController->buffers->setupPreviewPlanet(previewData);
    }
    if(camera) {
        camera->setPosition(0.0f, 0.0f, 5.0f);
        lockCamera();
    }
}

/*
** Update Preview
*/
void PreviewController::updatePreview(const PlanetData& data) {
    if(!isGeneratorActive) {
        emscripten_log(EM_LOG_CONSOLE, "Cannot update preview - not in generator mode!");
        return;
    }
    if(!bufferController || !bufferController->buffers) {
        emscripten_log(EM_LOG_CONSOLE, "Cannot update preview - not in generator mode!");
        return;
    }

    PlanetData previewData = data;
    previewData.id = -1;
    PlanetBuffer planetBuffer;
    planetBuffer.isPreview = true;
    bufferController->buffers->updatePreviewPlanet(previewData);
}

/*
** Cleanup Preview
*/
void PreviewController::cleanupPreview() {
    if(!isGeneratorActive) {
        emscripten_log(EM_LOG_CONSOLE, "No generator active");
        return;
    }
    if(bufferController && bufferController->buffers) {
        bufferController->buffers->cleanupPreviewPlanet();
    }
    if(isPreviewing) {
        unlockCamera();
    }

    isGeneratorActive = false;
    emscripten_log(EM_LOG_CONSOLE, "Cannot update preview - not in generator mode!");
}