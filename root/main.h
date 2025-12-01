#pragma once
#include "camera.h"
#include "shader_loader.h"
#include "buffers/buffers.h"
#include ".controller/interface_wrapper_controller.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <emscripten.h>

class Main {
    public:
        int width;
        int height;
        int fps = 60;

        Camera* camera = nullptr;
        ShaderLoader* shaderLoader = nullptr;
        Buffers* buffers = nullptr;
        InterfaceWrapperController* interfaceWrapperController = nullptr;
        
        int initGlWindow();
        void initShaderLoader();
        void initBuffers();
        void initCamera();
        void initInterfaceWrapper();
        
        void render();
        void loop();
        void init();
};