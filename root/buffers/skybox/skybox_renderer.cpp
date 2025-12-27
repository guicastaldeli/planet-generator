#include "skybox_renderer.h"
#include "../../_shaders/shader_controller.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


SkyboxRenderer::SkyboxRenderer() :
    vao(0),
    vbo(0),
    ebo(0),
    shaderProgram(0),
    timeUniform(0)
{}

SkyboxRenderer::~SkyboxRenderer() {
    cleanup();
}

void SkyboxRenderer::init() {
    createMesh();
    ShaderController* controller = ShaderController::getInstance();
    if(controller) shaderProgram = controller->getProgram();
    if(shaderProgram) timeUniform = glGetUniformLocation(shaderProgram, "uTime");
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
    if(shaderProgram == 0) return;

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);

    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1000.0f));

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint timeLoc = glGetUniformLocation(shaderProgram, "uTime");

    if(modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    if(viewLoc != -1) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &skyboxView[0][0]);
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
    if(ebo) glDeleteBuffers(1, &ebo);
}