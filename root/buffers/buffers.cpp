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
    BufferController* bufferController
) :
    camera(camera),
    shaderController(shaderController),
    bufferController(bufferController)
{}
Buffers::~Buffers() {
    for(auto& [type, v] : vaos) {
        glDeleteVertexArrays(1, &v);
        glDeleteBuffers(1, &vbos[type]);
        glDeleteBuffers(1, &ebos[type]);
    } 
}

/*
** Set Buffers
*/
void Buffers::set(BufferData::Type type) {
    if(vaos.find(type) != vaos.end()) return;

    BufferData::MeshData meshData = BufferData::GetMeshData(type);
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

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

    vaos[type] = vao;
    vbos[type] = vbo;
    ebos[type] = ebo;
    indexCounts[type] = meshData.indices.size();
}

/*
** Create Buffer for Planet
*/
void Buffers::createBufferForPlanet(const PlanetBuffer& planetBuffer) {
    set(planetBuffer.data.shape);
}

/*
** Render
*/
void Buffers::render() {
    glUseProgram(shaderController->shaderProgram);
    
    for(const auto& planetBuffer : planetBuffers) {
        auto it = vaos.find(planetBuffer.data.shape);
        if(it == vaos.end()) continue;

        glBindVertexArray(it->second);

        float orbitRadius = planetBuffer.data.distanceFromCenter;
        float orbitAngle = planetBuffer.data.orbitAngle.y;

        glm::vec3 position(
            orbitRadius * cos(orbitAngle),
            0.0f,
            orbitRadius * sin(orbitAngle)
        );

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, planetBuffer.data.currentRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(planetBuffer.data.size));

        unsigned int modelLoc = glGetUniformLocation(shaderController->shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        int isThisPlanetHovered = (
            bufferController->raycaster->selectedPlanetIndex == &planetBuffer - &planetBuffers[0]
        ) ? 1 : 0;
        GLuint hoverLoc = glGetUniformLocation(shaderController->shaderProgram, "isHovered");
        if(hoverLoc != -1) {
            glUniform1f(hoverLoc, (float)isThisPlanetHovered);
        }

        glDrawElements(
            GL_TRIANGLES,
            indexCounts[planetBuffer.data.shape],
            GL_UNSIGNED_INT,
            0
        );
    }

    glBindVertexArray(0);
}

/*
** Init
*/
void Buffers::init() {
    shaderController->initProgram();
    emscripten_log(EM_LOG_CONSOLE, "init buffers!");
}
