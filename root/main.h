#pragma once
#include "camera.h"
#include "shader_loader.h"
#include ".controller/buffer_controller.h"
#include ".controller/controls_wrapper_controller.h"
#include ".controller/info_wrapper_controller.h"
#include ".controller/generator_wrapper_controller.h"
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

        ControlsWrapperController* controlsWrapperController;
        InfoWrapperController* infoWapperController;
        GeneratorWrapperController* generatorWrapperController;
        
        int initGlWindow();        
        void render();
        void resize();
        void resizeCanvas();
        void loop();
        void init();
};
