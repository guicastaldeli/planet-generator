#pragma once
#include "camera.h"
#include "shader_loader.h"
#include ".controller/buffer_controller.h"
#include ".controller/interface_wrapper_controller.h"
#include ".controller/info_wrapper_controller.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <emscripten.h>

class Main {
    public:
        int width;
        int height;
        int fps = 60;

        Camera* camera;
        ShaderLoader* shaderLoader;
        BufferController* bufferController;
        InterfaceWrapperController* interfaceWrapperController;
        InfoWrapperController* infoWapperController;
        
        int initGlWindow();        
        void render();
        void loop();
        void init();
};