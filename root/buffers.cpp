#include "buffers.h"
#include "shader_controller.h"
#include <emscripten.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Buffers::Buffers(ShaderController* shaderController) :
    shaderController(shaderController)
{}
Buffers::~Buffers() {}

/*
** Set Buffers
*/
void Buffers::set() {
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        0.0f,  0.5f,  0.0f
    };
    GLuint indices[] = {
        4, 0, 1,
        4, 1, 2,
        4, 2, 3,
        4, 3, 0,
        0, 1, 2,
        0, 2, 3
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint posAttr = glGetAttribLocation(shaderController->shaderProgram, "aPos");
    glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(posAttr);

    glBindVertexArray(0);
}

/*
** Render
*/
void Buffers::render() {
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
