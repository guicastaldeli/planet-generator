#include "camera.h"
#include "main.h"
#include <emscripten.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Camera::set() {
    glUseProgram(shaderController->shaderProgram);
    setMatrix();

    const float radius = 3.0f;
    float time = (float) emscripten_get_now() / 1000.0f;
    float camX = sin(time) * radius;
    float camZ = cos(time) * radius;

    glm::mat4 view;
    view = glm::lookAt(
        glm::vec3(camX, 0.0, camZ),
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0)
    );
    int viewLoc = glGetUniformLocation(shaderController->shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void Camera::setMatrix() {
    glm::mat4 projMatrix;
    projMatrix = glm::perspective(glm::radians(45.0f), (float)main->width / (float)main->height, 0.1f, 100.0f);
    int projMatrixLoc = glGetUniformLocation(shaderController->shaderProgram, "projection");
    glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
}

void Camera::update() {
    set();
}

void Camera::init() {
    emscripten_log(EM_LOG_CONSOLE, "init camera!");
}