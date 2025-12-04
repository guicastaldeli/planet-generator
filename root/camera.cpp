#include "camera.h"
#include "main.h"
#include ".buffers/buffers.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include ".controller/buffer_controller.h"

Camera::Camera(
    Main* main, 
    ShaderController* shaderController, 
    BufferController* bufferController
) : 
    main(main), 
    shaderController(shaderController),
    bufferController(bufferController),
    position(0.0f, 0.0f, 3.0f),
    target(0.0f, 0.0f, 0.0f),
    up(0.0f, 1.0f, 0.0f),
    right(1.0f, 0.0f, 0.0f),
    yaw(-90.0f),
    pitch(0.0f),
    isRotating(false),
    isPanning(false),
    lastMouseX(0.0),
    lastMouseY(0.0),
    zoomLevel(45.0f),
    rotationSpeed(0.1f),
    panSpeed(0.001f),
    zoomSpeed(0.1f),
    panningLocked(false),
    rotationLocked(false),
    savedPosition(0.0f, 0.0f, 3.0f),
    savedTarget(0.0f, 0.0f, 0.0f),
    isFollowingPlanet(false),
    followingPlanetIndex(-1),
    followingPlanetOffset(0.0f)
{}
Camera::~Camera() {}

/*
** Set Camera
*/
void Camera::set() {
    glUseProgram(shaderController->shaderProgram);
    
    glm::mat4 projMatrix = getProjectionMatrix();
    int projMatrixLoc = glGetUniformLocation(shaderController->shaderProgram, "projection");
    glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

    glm::mat4 viewMatrix = getViewMatrix();
    int viewLoc = glGetUniformLocation(shaderController->shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

/*
**
*** Controls
**
*/
void Camera::rotate(float deltaX, float deltaY) {
    if(rotationLocked) return;

    yaw += deltaX * rotationSpeed;
    pitch -= deltaY * rotationSpeed;

    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    updateVectors();
}

void Camera::pan(float deltaX, float deltaY) {
    if(panningLocked) return;

    glm::vec3 panOffset = (
        -right * deltaX +
        up * deltaY
    ) * panSpeed;

    position += panOffset;
    target += panOffset;
}

void Camera::zoom(float delta) {
    zoomLevel += delta * zoomSpeed;
    if(zoomLevel < 1.0f) zoomLevel = 1.0f;
    if(zoomLevel > 90.0f) zoomLevel = 90.0f;
}

void Camera::reset() {
    position = glm::vec3(0.0f, 0.0f, 3.0f);
    target = glm::vec3(0.0f, 0.0f, 0.0f);
    up = glm::vec3(0.0, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    zoomLevel = 45.0f;
    updateVectors();
}

void Camera::updateVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    float distance = glm::length(position - target);
    position = target - front * distance;

    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, front));
}

/*
** Save Current Position
*/
void Camera::saveCurrentPos() {
    savedPosition = position;
    savedTarget = target;
    emscripten_log(EM_LOG_CONSOLE, "Camera position saved: %.2f, %.2f, %.2f", 
                   savedPosition.x, savedPosition.y, savedPosition.z);
}

/*
** Zoom to Object
*/
void Camera::zoomToObj(const glm::vec3& planetPosition, float planetSize) {
    if(panningLocked) saveCurrentPos();
    target = planetPosition;

    if(bufferController && bufferController->getSelectedPlanet()) {
        planetSize = bufferController->getSelectedPlanet()->data.size;
        followingPlanetIndex = bufferController->getSelectedPlanetIndex();
    }

    float zoomDistance = planetSize * 3.0f;
    glm::vec3 directionToPlanet = glm::normalize(planetPosition - position);
    followingPlanetOffset = -directionToPlanet * zoomDistance;
    position = planetPosition + followingPlanetOffset;
    up = glm::vec3(0.0f, 1.0f, 0.0f);

    isFollowingPlanet = true;
    updateVectors();
    lockPanning(true);
    
    emscripten_log(EM_LOG_CONSOLE, "Zoomed to planet at (%.2f, %.2f, %.2f)", 
                   position.x, position.y, position.z);
}

/*
** Update Following
*/
void Camera::updateFollowing() {
    if(!isFollowingPlanet || !bufferController || followingPlanetIndex == -1) {
        return;
    }

    const PlanetBuffer* planet = bufferController->getSelectedPlanet();
    /*
    if(!planet) {
        resetToSavedPos();
        return;
    }
        */

    target = planet->worldPos;
    position = target + followingPlanetOffset;
    updateVectors();
}

/*
** Reset to Position
*/
void Camera::resetToSavedPos() {
    if(!panningLocked || !rotationLocked) return;

    position = savedPosition;
    target = savedTarget;
    updateVectors();

    isFollowingPlanet = false;
    followingPlanetIndex = -1;

    lockPanning(false);
    emscripten_log(EM_LOG_CONSOLE, "Reset to saved position, panning unlocked");
}

/*
** Lock
*/
void Camera::lockPanning(bool lock) {
    panningLocked = lock;
    emscripten_log(EM_LOG_CONSOLE, "Panning %s", lock ? "locked" : "unlocked");
}

void Camera::lockRotation(bool lock) {
    rotationLocked = lock;
    emscripten_log(EM_LOG_CONSOLE, "Rotation %s", lock ? "locked" : "unlocked");
}

/*
**
*** Mouse Controls
**
*/
void Camera::handleMouseDown(double x, double y, int button) {
    lastMouseX = x;
    lastMouseY = y;
    if(button == 0) {
        isPanning = true;
    } else if(button == 2) {
        isRotating = true;
    }
}

void Camera::handleMouseUp(double x, double y, int button) {
    if(button == 0) {
        isPanning = false;
    } else if(button == 2) {
        isRotating = false;
    }
}

void Camera::handleMouseMove(double x, double y) {
    double deltaX = x - lastMouseX;
    double deltaY = y - lastMouseY;

    if(isRotating) {
        rotate(deltaX, deltaY);
    } else if(isPanning) {
        pan(deltaX, deltaY);
    }

    lastMouseX = x;
    lastMouseY = y;
}

void Camera::handleMouseScroll(double delta) {
    zoom(delta);
}

/*
**
*** Events
**
*/
EM_BOOL mouseCallback(
    int eventType, 
    const EmscriptenMouseEvent* e,
    void* userData
) {
    Camera* camera = static_cast<Camera*>(userData);
    if (!camera || !camera->main || !camera->bufferController) {
        return EM_TRUE;
    }
    switch(eventType) {
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
            camera->handleMouseDown(e->clientX, e->clientY, e->button);
            if(e->button == 0) {
                if(camera->bufferController) {
                    camera->bufferController->updatePlanetPositions();
                    camera->bufferController->handleRaycasterClick(
                        e->clientX,
                        e->clientY
                    );
                }
            }
            break;
        case EMSCRIPTEN_EVENT_MOUSEUP:
            camera->handleMouseUp(e->clientX, e->clientY, e->button);
            break;
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
            camera->handleMouseMove(e->clientX, e->clientY);

            if(camera->bufferController) {
                camera->bufferController->handleRaycasterRender(
                    e->clientX,
                    e->clientY
                );
            }
            break;
    }
    return EM_TRUE;
}

EM_BOOL wheelCallback(
    int eventType,
    const EmscriptenWheelEvent* e,
    void* userData
) {
    Camera* camera = static_cast<Camera*>(userData);
    camera->handleMouseScroll(e->deltaY);
    return EM_TRUE;
}

EM_BOOL keyCallback(
    int eventType,
    const EmscriptenKeyboardEvent* e,
    void* userData
) {
    if(eventType == EMSCRIPTEN_EVENT_KEYDOWN) {
        Camera* camera = static_cast<Camera*>(userData);
        if(strcmp(e->key, "Escape") == 0) {
            camera->resetToSavedPos();
            return EM_TRUE;
        }
    }
    return EM_FALSE;
}

glm::mat4 Camera::getViewMatrix() {
    glm::mat4 view = glm::lookAt(position, target, up);
    return view;
}

glm::mat4 Camera::getProjectionMatrix() {
    glm::mat4 projMatrix;
    projMatrix = glm::perspective(
        glm::radians(zoomLevel),
        (float)main->width / (float)main->height,
        0.1f, 
        100.0f
    );
    return projMatrix;
}

void Camera::setEvents() {
    emscripten_set_mousedown_callback("#ctx", this, 1, mouseCallback);
    emscripten_set_mouseup_callback("#ctx", this, 1, mouseCallback);
    emscripten_set_mousemove_callback("#ctx", this, 1, mouseCallback);
    emscripten_set_wheel_callback("#ctx", this, 1, wheelCallback);
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, 1, keyCallback);
}

void Camera::update() {
    if(isFollowingPlanet) updateFollowing();
    set();
}

void Camera::init() {
    emscripten_log(EM_LOG_CONSOLE, "init camera!");
    setEvents();
}