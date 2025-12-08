#include "preview_controller.h"
#include "../.buffers/buffers.h"
#include "../_utils/default_data.h"
#include <emscripten.h>

PreviewController::PreviewController(BufferController* bufferController, Camera* camera) :
    bufferController(bufferController),
    camera(camera),
    isPreviewing(false),
    isGeneratorActive(false)
{};
PreviewController::~PreviewController() {};

/*
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

/*
** Preview
*/
void PreviewController::preview() {
    isGeneratorActive = true;
    isPreviewing = true;
    if(isPreviewing && isGeneratorActive) {
        camera->saveCurrentPosBefore();
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

/*
** Start Generator Preview
*/
void PreviewController::startGeneratorPreview() {
    isGeneratorActive = true;
    isPreviewing = true;

    if(camera) {
        emscripten_console_log("camera");
        camera->saveCurrentPosBefore();
        preview();
    } else {
        emscripten_console_log("no camera");
        preview();
    }

    PlanetData previewData;
    if(
        bufferController && 
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
        previewData.position = defaultDataPtr->position;
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
    if(!isPreviewing && !isGeneratorActive) return;

    if(camera) {
        camera->resetToSavedPos();
        unlockCamera();
    }
    if(isPreviewing) {
        exitPreview();
    }
    if(bufferController && bufferController->buffers) {
        bufferController->buffers->cleanupPreviewPlanet();
        bufferController->buffers->setPreviewMode(false);
    }

    isGeneratorActive = false;
    isPreviewing = false;

    emscripten_log(EM_LOG_CONSOLE, "Cleanup completed!");
}