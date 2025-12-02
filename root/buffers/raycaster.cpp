#include "raycaster.h"
#include "../camera.h"
#include "buffers.h"
#include "../main.h"
#include "../.controller/shader_controller.h"
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
    isIntersecting(false),
    selectedPlanetIndex(-1)
{}
Raycaster::~Raycaster() {}

bool Raycaster::checkIntersection(
    double mouseX,
    double mouseY,
    int viewportWidth,
    int viewportHeight,
    const glm::vec3& planetPosition,
    float planetSize,
    int planetIndex
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

    bool intersects = aabb(rayWorld, planetPosition, planetSize);
    if(intersects) selectedPlanetIndex = planetIndex; 

    return intersects;
}

/*
** AABB 
*/
bool Raycaster::aabb(
    glm::vec3 rayWorldDir,
    const glm::vec3& planetPosition,
    float planetSize
) {
    if(!buffers) return false;
    
    glm::vec3 aabbMin = planetPosition - glm::vec3(planetSize);
    glm::vec3 aabbMax = planetPosition + glm::vec3(planetSize);

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
bool Raycaster::handleClick(
    double x, 
    double y, 
    int viewportWidth, 
    int viewportHeight,
    const glm::vec3& planetPosition,
    float planetSize,
    int planetIndex
) {
    bool intersects = checkIntersection(
        x, 
        y, 
        viewportWidth, 
        viewportHeight,
        planetPosition,
        planetSize,
        planetIndex
    );
    if(intersects) {
        camera->zoomToObj(planetPosition, planetSize);
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
void Raycaster::render(
    double x, 
    double y,
    const glm::vec3& planetPosition,
    float planetSize,
    int planetIndex
) {
    isIntersecting = checkIntersection(
        x, 
        y, 
        main->width, 
        main->height,
        planetPosition,
        planetSize,
        planetIndex
    );
    GLuint hoverLoc = glGetUniformLocation(shaderController->shaderProgram, "isHovered");
    if(hoverLoc != -1) {
        glUniform1f(hoverLoc, isIntersecting ? 1.0f : 0.0f);
    }
    if(isIntersecting) {
        selectedPlanetIndex = planetIndex;
    }
}