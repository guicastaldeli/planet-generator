#include "preview_controller.h"

PreviewController::PreviewController(BufferController* bufferController, Camera* camera) :
    bufferController(bufferController),
    camera(camera) 
{};
PreviewController::~PreviewController() {};

/*
 *
 * Lock and Unlock Camera
 * 
 */
void PreviewController::lockCamera() {
    camera->lockPanning(true);
    camera->lockRotation(true);
}

void PreviewController::unlockCamera() {
    camera->lockPanning(false);
    camera->lockRotation(false);
}

/*
** Positionate Camera
*/
void PreviewController::positionateCamera() {

}

/*
** Preview
*/
void PreviewController::preview() {
    lockCamera();
}

void PreviewController::exitPreview() {
    unlockCamera();
}

void PreviewController::init() {

}