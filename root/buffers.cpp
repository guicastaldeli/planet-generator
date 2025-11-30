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
        0.0f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, (float)emscripten_get_now() / 1000.f, glm::vec3(0.0f, 1.0f, 0.0f));
    unsigned int modelLoc = glGetUniformLocation(shaderController->shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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
