#include "buffers.h"
#include "../.controller/shader_controller.h"
#include "../camera.h"
#include <emscripten.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Buffers::Buffers(
    Camera* camera, 
    ShaderController* shaderController,
    BufferData::Type type
) :
    camera(camera),
    shaderController(shaderController),
    bufferType(type),
    indexCount(0)
{}
Buffers::~Buffers() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

/*
** Set Buffers
*/
void Buffers::set() {
    BufferData::MeshData meshData = BufferData::GetMeshData(bufferType);
    indexCount = meshData.indices.size();
    minBounds = meshData.minBounds;
    maxBounds = meshData.maxBounds;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER, 
        meshData.vertices.size() * sizeof(float),
        meshData.vertices.data(), 
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, 
        meshData.indices.size() * sizeof(GLuint),
        meshData.indices.data(), 
        GL_STATIC_DRAW
    );

    GLuint posAttr = glGetAttribLocation(shaderController->shaderProgram, "aPos");
    glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(posAttr);

    glBindVertexArray(0);
}

/*
** Render Orbit
*/
void Buffers::setOrbit() {
    for(const auto& planetBuffer : planetBuffers) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(
            model, 
            planetBuffer.data.currentRotation.x,
            glm::vec3(1.0f, 0.0f, 0.0f)
        );
        model = glm::rotate(
            model, 
            planetBuffer.data.currentRotation.y,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        model = glm::rotate(
            model, 
            planetBuffer.data.currentRotation.z,
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        if(planetBuffer.data.position != 0) {
            float orbitRadius = planetBuffer.data.distanceFromCenter;
            float angle = planetBuffer.data.orbitAngle.y;

            glm::mat4 orbitTransform = glm::mat4(1.0f);
            orbitTransform = glm::rotate(
                orbitTransform,
                angle,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            orbitTransform = glm::translate(
                orbitTransform,
                glm::vec3(orbitRadius, 0.0f, 0.0f)
            );
            model = orbitTransform * model;
        }

        model = glm::scale(model, glm::vec3(planetBuffer.data.size));
    }
}

/*
** Render
*/
void Buffers::render() {
    if(indexCount == 0) return;
    
    glUseProgram(shaderController->shaderProgram);
    glBindVertexArray(vao);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, (float)emscripten_get_now() / 1000.f, glm::vec3(0.0f, 1.0f, 0.0f));
    unsigned int modelLoc = glGetUniformLocation(shaderController->shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/*
** Init
*/
void Buffers::init() {
    shaderController->initProgram();
    set();
    emscripten_log(EM_LOG_CONSOLE, "init buffers!");
}
