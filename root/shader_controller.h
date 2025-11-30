#pragma once
#include <GLES3/gl3.h>
#include "shader_loader.h"

class ShaderController {
    public:
        GLuint fragShader;
        GLuint vertexShader;
        GLuint shaderProgram;

        void checkStatus();
        void load();
        void initProgram();
};