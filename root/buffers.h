#pragma once
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>

class ShaderController;
class Buffers {
    private:
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        float vertices[6];
        
        ShaderController* shaderController;
        
        void set();
        
    public:
        Buffers(ShaderController* shaderController);
        ~Buffers();

        void render();
        void init();
};