#include "preview_controller.h"
#include "../.buffers/buffers.h"
#include <emscripten.h>

PreviewController::PreviewController(BufferController* bufferController, Camera* camera) :
    bufferController(bufferController),
    camera(camera),
    isPreviewing(false),
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
** Preview
*/
void PreviewController::preview() {
    isGeneratorActive = true;
    isPreviewing = true;
    if(isPreviewing && isGeneratorActive) {
        lockCamera();
        camera->setPosition(0.0f, 0.0f, 2.0f, false);
        camera->target = glm::vec3(10.0f, 0.0f, -60.0f);
        camera->updateVectors();
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

/*
** Start Generator Preview
*/
void PreviewController::startGeneratorPreview() {
    if(isGeneratorActive) cleanupPreview();

    if(camera) {
        emscripten_console_log("camera");
        camera->saveCurrentPosBefore();
        preview();
    } else {
        emscripten_console_log("no camera");
        preview();
    }

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
}

/*
** Update Preview
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

/*
** Cleanup Preview
*/
void PreviewController::cleanupPreview() {
    if(camera) {
        camera->resetToSavedPos();
        camera->releaseCamera();
    }
    if(isPreviewing) {
        exitPreview();
    }
    if(bufferController && bufferController->buffers) {
        bufferController->buffers->cleanupPreviewPlanet();
    }

    isGeneratorActive = false;
    isPreviewing = false;

    emscripten_log(EM_LOG_CONSOLE, "Cleanup completed!");
}