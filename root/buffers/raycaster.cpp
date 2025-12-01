#include "raycaster.h"
#include "camera.h"
#include "buffers/buffers.h"
#include "main.h"
#include ".controller/shader_controller.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <glm/glm.hpp>

Raycaster::Raycaster(
    Main* main,
    Camera* camera, 
    Buffers* buffers,
    ShaderController* shaderController
) :
    main(main),
    camera(camera),
    buffers(buffers),
    shaderController(shaderController),
    isIntersecting(false)
{}
Raycaster::~Raycaster() {}

bool Raycaster::checkIntersection(
    double mouseX,
    double mouseY,
    int viewportWidth,
    int viewportHeight
) {
    float x = (2.0f * mouseX) / viewportWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / viewportHeight;

    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::mat4 projMatrix = glm::perspective(
        glm::radians(camera->zoomLevel),
        (float)viewportWidth / (float)viewportHeight,
        0.1f,
        100.0f
    );
    glm::mat4 invProj = glm::inverse(projMatrix);
    glm::vec4 rayEye = invProj * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::mat4 view = glm::lookAt(
        glm::vec3(camera->position),
        glm::vec3(camera->target),
        glm::vec3(camera->up)
    );
    glm::mat4 invView = glm::inverse(view);
    glm::vec4 rayWorld4 = invView * rayEye;
    glm::vec3 rayWorld = glm::normalize(rayWorld4);

    return aabb(rayWorld);
}

/*
** AABB 
*/
bool Raycaster::aabb(glm::vec3 rayWorldDir) {
    if(!buffers) return;
    glm::vec3 aabbMin = buffers->getMinBounds();
    glm::vec3 aabbMax = buffers->getMaxBounds();

    glm::vec3 rayOrigin = camera->position;
    glm::vec3 invDir = 1.0f / rayWorldDir;
    glm::vec3 t0 = (aabbMin - rayOrigin) * invDir;
    glm::vec3 t1 = (aabbMax - rayOrigin) * invDir;

    glm::vec3 tmin = glm::min(t0, t1);
    glm::vec3 tmax = glm::max(t0, t1);
    float tminMax = glm::max(glm::max(tmin.x, tmin.y), tmin.z);
    float tmaxMin = glm::min(glm::min(tmax.x, tmax.y), tmax.z);
    
    bool intersects = (tmaxMin >= tminMax) && (tmaxMin > 0);
    return intersects;
}

/*
** Handle Click
*/
bool Raycaster::handleClick(double x, double y, int viewportWidth, int viewportHeight) {
    bool intersects = checkIntersection(x, y, viewportWidth, viewportHeight);
    if(intersects && !camera->isPanningLocked()) {
        camera->zoomToObj();
        return true;
    }
    return false;
}

bool Raycaster::isMouseIntersecting() const {
    return isIntersecting;
}

/*
** Render
*/
void Raycaster::render(double x, double y) {
    isIntersecting = checkIntersection(x, y, main->width, main->height);
    GLuint hoverLoc = glGetUniformLocation(shaderController->shaderProgram, "isHovered");
    if (hoverLoc != -1) {
        glUniform1f(hoverLoc, isIntersecting ? 1.0f : 0.0f);
    }
}