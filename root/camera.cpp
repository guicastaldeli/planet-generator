#include "camera.h"
#include "main.h"
#include <emscripten.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Camera::set() {
    glUseProgram(shaderController->shaderProgram);

    glm::mat4 projMatrix;
    projMatrix = glm::perspective(glm::radians(45.0f), (float)main->width / (float)main->height, 0.1f, 100.0f);
    int projMatrixLoc = glGetUniformLocation(shaderController->shaderProgram, "projection");
    glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    int viewLoc = glGetUniformLocation(shaderController->shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void Camera::update() {
    set();
}

void Camera::init() {
    emscripten_log(EM_LOG_CONSOLE, "init camera!");
}