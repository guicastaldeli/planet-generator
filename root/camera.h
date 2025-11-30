#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>
#include <GLFW/glfw3.h>
#include <emscripten.h>
#include "shader_controller.h"

class Main;
class Camera {
    private:
        Main* main;
        ShaderController* shaderController;

        void set();

    public:
        void init();
        void update();
        Camera(Main* mainPtr, ShaderController* shaderPtr) : main(mainPtr), shaderController(shaderPtr) {}
};

#endif // CAMERA_H