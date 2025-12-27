#include "skybox_renderer.h"
#include "../../_shaders/shader_controller.h"
#include <iostream>

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

void ShaderRenderer::init() {
    createMesh();
    shaderProgram = compileShaderProgram();
    if(shaderProgram) timeUniform = glGenUniformLocation(shaderProgram, "uTime");
}

/**
 * Create Mesh
 */
void SkyboxRenderer::createMesh() {
    const auto& sphereData = BufferData::GetMeshData(BufferData::Type::SPHERE);

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

    glBindBuffer(GL_ELEMENT_ARRAY, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sphereData.indices.size() * sizeof(GLuint),
        sphereData.indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0).
    glBindVertexArray(0);
}

/**
 * Render
 */
void SkyboxRenderer::render(const glm::mat4& view, const glm::mat4& projection, float time) {
    if(shaderProgram == 0) return;

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);

    glm::mat4 view = glm::mat4(glm::mat3(view));

    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint timeLoc = glGetUniformLocation(shaderProgram, "uTime");

    if(viewLoc != -1) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &skyboxView[0][0]);
    if(projLoc != -1) glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);
    if(timeLOc != -1) glUniform1f(timeLoc, time);

    glDepthMask(GL_FALSE);
    glDrawElements(
        GL_TRIANGLES,
        sphereData.indices.size(),
        GL_UNSIGNED_INT,
        0
    );
    glDepthMask(GL_TRUE);
    glBindVertexArray(0);
}

void SkyboxRenderer::cleanup() {
    if(vao) glDeleteVertexArrays(1, &vao);
    if(vbo) glDeleteBuffers(1, &vbo);
    if(evo) glDeleteBuffers(1, &ebo);
}