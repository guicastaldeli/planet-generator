#include "camera.h"
#include "main.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(Main* main, ShaderController* shaderController) : 
    main(main), 
    shaderController(shaderController),
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
    zoomSpeed(0.1f)
{
    raycaster = new Raycaster(
        main, 
        this, 
        main->buffers, 
        shaderController
    );
}
Camera::~Camera() {}

/*
** Set Camera
*/
void Camera::set() {
    glUseProgram(shaderController->shaderProgram);
    
    glm::mat4 projMatrix;
    projMatrix = glm::perspective(
        glm::radians(zoomLevel),
        (float)main->width / (float)main->height,
        0.1f, 
        100.0f
    );
    int projMatrixLoc = glGetUniformLocation(shaderController->shaderProgram, "projection");
    glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

    glm::mat4 view = glm::lookAt(position, target, up);
    int viewLoc = glGetUniformLocation(shaderController->shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

/*
**
*** Controls
**
*/
void Camera::rotate(float deltaX, float deltaY) {
    yaw += deltaX * rotationSpeed;
    pitch -= deltaY * rotationSpeed;

    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    updateVectors();
}

void Camera::pan(float deltaX, float deltaY) {
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
    switch(eventType) {
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
            camera->handleMouseDown(e->clientX, e->clientY, e->button);
            break;
        case EMSCRIPTEN_EVENT_MOUSEUP:
            camera->handleMouseUp(e->clientX, e->clientY, e->button);
            break;
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
            camera->handleMouseMove(e->clientX, e->clientY);
            camera->raycaster->render(e->clientX, e->clientY);
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

void Camera::setEvents() {
    emscripten_set_mousedown_callback("#ctx", this, 1, mouseCallback);
    emscripten_set_mouseup_callback("#ctx", this, 1, mouseCallback);
    emscripten_set_mousemove_callback("#ctx", this, 1, mouseCallback);
    emscripten_set_wheel_callback("#ctx", this, 1, wheelCallback);
}

void Camera::update() {
    set();
}

void Camera::init() {
    emscripten_log(EM_LOG_CONSOLE, "init camera!");
    setEvents();
}