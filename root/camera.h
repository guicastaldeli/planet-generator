#pragma once
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
        void setMatrix();
    public:
        void init();
        void update();
        Camera(Main* mainPtr, ShaderController* shaderPtr);
        ~Camera();
};