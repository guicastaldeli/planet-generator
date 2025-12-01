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
    BufferTypes::Type type
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
    BufferTypes::MeshData meshData = BufferTypes::GetMeshData(bufferType);
    indexCount = meshData.indices.size();

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

    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
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
