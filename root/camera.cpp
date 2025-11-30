#include "camera.h"

void Camera::set() {
    glViewport(0, 0, main->width, main->height);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}