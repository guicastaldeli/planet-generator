#include "shader_loader.h"
#include "buffers.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <emscripten.h>

class Main {
    public:
        int width;
        int height;
        int fps = 60;
        Buffers* buffers;
        
        int initGlWindow();
        void initShaderLoader();
        void initBuffers();
        
        void render();
        void loop();
        void init();
};