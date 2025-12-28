#include "skybox_renderer.h"
#include "../../_shaders/shader_controller.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <emscripten/emscripten.h>
#include <emscripten.h>
#include <emscripten/html5.h>

SkyboxRenderer::SkyboxRenderer() :
    vao(0),
    vbo(0),
    ebo(0),
    shaderProgram(0),
    timeUniform(0),
    starRenderer(nullptr)
{}
SkyboxRenderer::~SkyboxRenderer() {
    cleanup();
}

void SkyboxRenderer::init() {
    createMesh();
    ShaderController* shaderController = ShaderController::getInstance();
    if(shaderController) shaderProgram = shaderController->getProgram();
    if(shaderProgram) timeUniform = glGetUniformLocation(shaderProgram, "uTime");

    starRenderer = new StarRenderer();
    starRenderer->init();
}

/**
 * Create Mesh
 */
void SkyboxRenderer::createMesh() {
    const auto& sphereData = BufferData::GetMeshData(BufferData::Type::SPHERE);
    if(sphereData.vertices.empty() || sphereData.indices.empty()) {
        printf("ERROR: Invalid sphere data for skybox\n");
        return;
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sphereData.vertices.size() * sizeof(float),
        sphereData.vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sphereData.indices.size() * sizeof(GLuint),
        sphereData.indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    indexCount = sphereData.indices.size();

    glBindVertexArray(0);
}

/**
 * Render
 */
void SkyboxRenderer::render(const glm::mat4& view, const glm::mat4& proj, float time) {
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);

    GLuint shaderTypeLoc = glGetUniformLocation(shaderProgram, "shaderType");
    if(shaderTypeLoc != -1) glUniform1f(shaderTypeLoc, 1.0f);

    glm::mat4 model = glm::mat4(1.0f);
    model[0][0] = 10.0f;
    model[1][1] = 10.0f;
    model[2][2] = 10.0f;
    glm::mat4 viewMat = glm::mat4(glm::mat3(view));

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint timeLoc = glGetUniformLocation(shaderProgram, "uTime");
    if(modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    if(viewLoc != -1) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMat[0][0]);
    if(projLoc != -1) glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);
    if(timeLoc != -1) glUniform1f(timeLoc, time);

    glDepthMask(GL_FALSE);
    glDrawElements(
        GL_TRIANGLES,
        indexCount,
        GL_UNSIGNED_INT,
        0
    );
    glDepthMask(GL_TRUE);
    glBindVertexArray(0);
}

void SkyboxRenderer::cleanup() {
    if(vao) glDeleteVertexArrays(1, &vao);
    if(vbo) glDeleteBuffers(1, &vbo);
}