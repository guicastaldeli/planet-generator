#pragma once
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include "shader_controller.h"

class Buffers {
    private:
        GLuint vao;
        GLuint vbo;
        float vertices[6];

        ShaderController* shaderController;
        
        void set();

    public:
        Buffers();
        ~Buffers();

        void render();
        void init();
};