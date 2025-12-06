#include "raycaster.h"
#include "../camera.h"
#include "buffers.h"
#include "../main.h"
#include "../.controller/shader_controller.h"
#include "../.controller/info_wrapper_controller.h"
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
    int planetIndex,
    BufferData::Type shapeType
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
        camera->position,
        camera->target,
        camera->up
    );
    glm::mat4 invView = glm::inverse(view);

    glm::vec4 rayWorld4 = invView * rayEye;
    glm::vec3 rayWorld = glm::normalize(glm::vec3(rayWorld4));
    bool intersects = false;
    switch(shapeType) {
        case BufferData::Type::SPHERE:
            intersects = sphereIntersection(rayWorld, planetPosition, planetSize);
            break;
        case BufferData::Type::CUBE:
            intersects = cubeIntersection(rayWorld, planetPosition, planetSize);
            break;
        case BufferData::Type::TRIANGLE:
            intersects = triangleIntersection(rayWorld, planetPosition, planetSize);
            break;
        default:
            intersects = meshIntersection(rayWorld, planetPosition, planetSize, shapeType);
            break;
    }
    return intersects;
}

/*
**
*** Intersection
**
*/
bool Raycaster::sphereIntersection(
    glm::vec3 rayWorldDir,
    const glm::vec3& planetPosition,
    float planetSize
) {
    glm::vec3 rayOrigin = camera->position;
    glm::vec3 sphereCenter = planetPosition;
    float sphereRadius = planetSize;

    glm::vec3 oc = rayOrigin - sphereCenter;
    float a = glm::dot(rayWorldDir, rayWorldDir);
    float b = 2.0f * glm::dot(oc, rayWorldDir);
    float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;

    float discriminant = b * b - 4.0f * a * c;
    return discriminant >= 0;
}

bool Raycaster::cubeIntersection(
    glm::vec3 rayWorldDir,
    const glm::vec3& planetPosition,
    float planetSize
) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, planetPosition);
    model = glm::scale(model, glm::vec3(planetSize * 2.0f));

    glm::mat4 invModel = glm::inverse(model);
    glm::vec3 localRayOrigin = glm::vec3(invModel * glm::vec4(camera->position, 1.0f));
    glm::vec3 localRayDir = glm::vec3(invModel * glm::vec4(rayWorldDir, 0.0f));

    glm::vec3 aabbMin = glm::vec3(-0.5f, -0.5f, -0.5f);
    glm::vec3 aabbMax = glm::vec3(0.5f, 0.5f, 0.5f);
    
    glm::vec3 invDir = 1.0f / localRayDir;
    glm::vec3 t0 = (aabbMin - localRayOrigin) * invDir;
    glm::vec3 t1 = (aabbMax - localRayOrigin) * invDir;

    glm::vec3 tmin = glm::min(t0, t1);
    glm::vec3 tmax = glm::max(t0, t1);

    float tminMax = glm::max(glm::max(tmin.x, tmin.y), tmin.z);
    float tmaxMin = glm::min(glm::min(tmax.x, tmax.y), tmax.z);
    
    return tmaxMin >= tminMax && tmaxMin > 0;
}

bool Raycaster::triangleIntersection(
    glm::vec3 rayWorldDir,
    const glm::vec3& planetPosition,
    float planetSize
) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, planetPosition);
    model = glm::scale(model, glm::vec3(planetSize * 2.0f));

    glm::mat4 invModel = glm::inverse(model);
    glm::vec3 localRayOrigin = glm::vec3(invModel * glm::vec4(camera->position, 1.0f));
    glm::vec3 localRayDir = glm::vec3(invModel * glm::vec4(rayWorldDir, 0.0f));
    localRayDir = glm::normalize(localRayDir);

    glm::vec3 v0 = glm::vec3(-0.3f, -0.3f, -0.3f);
    glm::vec3 v1 = glm::vec3(0.3f, -0.3f, -0.3f);
    glm::vec3 v2 = glm::vec3(0.3f, -0.3f, 0.3f);
    glm::vec3 v3 = glm::vec3(-0.3f, -0.3f, 0.3f);
    glm::vec3 v4 = glm::vec3(0.0f, 0.3f, 0.0f);

    glm::vec3 triangles[6][3] = {
        {v4, v0, v1},
        {v4, v1, v2},
        {v4, v2, v3},
        {v4, v3, v0}, 
        {v0, v1, v2},
        {v0, v2, v3}
    };
    for(int i = 0; i < 6; i++) {
        if(rayTriangleIntersection(
            localRayOrigin,
            localRayDir,
            triangles[i][0],
            triangles[i][1],
            triangles[i][2]
        )) {
            return true;
        }
    }

    return false;
}

bool Raycaster::meshIntersection(
    glm::vec3 rayWorldDir,
    const glm::vec3& planetPosition,
    float planetSize,
    BufferData::Type shapeType
) {
    const BufferData::MeshData& meshData = BufferData::GetMeshData(shapeType);
    const std::vector<float>& vertices = meshData.vertices;
    const std::vector<GLuint>& indices = meshData.indices;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, planetPosition);
    model = glm::scale(model, glm::vec3(planetSize));

    glm::mat4 invModel = glm::inverse(model);
    glm::vec3 localRayOrigin = glm::vec3(invModel * glm::vec4(camera->position, 1.0f));
    glm::vec3 localRayDir = glm::vec3(invModel * glm::vec4(rayWorldDir, 0.0f));
    localRayDir = glm::normalize(localRayDir);

    for(size_t i = 0; i < indices.size(); i += 3) {
        GLuint i0 = indices[i];
        GLuint i1 = indices[i + 1];
        GLuint i2 = indices[i + 2];
        
        glm::vec3 v0 = glm::vec3(
            vertices[i0 * 3],
            vertices[i0 * 3 + 1],
            vertices[i0 * 3 + 2]
        );
        glm::vec3 v1 = glm::vec3(
            vertices[i1 * 3],
            vertices[i1 * 3 + 1],
            vertices[i1 * 3 + 2]
        );
        glm::vec3 v2 = glm::vec3(
            vertices[i2 * 3],
            vertices[i2 * 3 + 1],
            vertices[i2 * 3 + 2]
        );
        if(rayTriangleIntersection(
            localRayOrigin, 
            localRayDir, 
            v0, v1, v2)
        ) {
            return true;
        }
    }

    return false;
}

bool Raycaster::rayTriangleIntersection(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDir,
    const glm::vec3& v0,
    const glm::vec3& v1,
    const glm::vec3& v2
) {
    const float EPSILON = 0.0000001f;

    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(rayDir, edge2);

    float a = glm::dot(edge1, h);
    if(a > -EPSILON && a < EPSILON) return false;

    float f = 1.0f / a;
    glm::vec3 s = rayOrigin - v0;
    float u = f * glm::dot(s, h);
    if(u < 0.0f || u > 1.0f) return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(rayDir, q);
    if(v < 0.0f || u + v > 1.0f) return false;

    float t = f * glm::dot(edge2, q);
    return t > EPSILON;
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
    int planetIndex,
    BufferData::Type shapeType
) {
    bool intersects = checkIntersection(
        x, 
        y, 
        viewportWidth, 
        viewportHeight,
        planetPosition,
        planetSize,
        planetIndex,
        shapeType
    );
    if(intersects) {
        camera->zoomToObj(planetPosition, planetSize);

        if(buffers && planetIndex >= 0 && planetIndex < buffers->planetBuffers.size()) {
            auto& planet = buffers->planetBuffers[planetIndex];
            display(
                planet.data.name.c_str(),
                planet.data.name.c_str()
            );
        }
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
    int planetIndex,
    BufferData::Type shapeType
) {
    isIntersecting = checkIntersection(
        x, 
        y, 
        main->width, 
        main->height,
        planetPosition,
        planetSize,
        planetIndex,
        shapeType
    );
    GLuint hoverLoc = glGetUniformLocation(shaderController->shaderProgram, "isHovered");
    if(hoverLoc != -1) {
        glUniform1f(hoverLoc, isIntersecting ? 1.0f : 0.0f);
    }
    if(isIntersecting) {
        selectedPlanetIndex = planetIndex;
    }
}