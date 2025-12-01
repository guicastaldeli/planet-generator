#pragma once
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>

class Camera;
class ShaderController;
class Buffers {
    private:
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        float vertices[6];
        
        Camera* camera;
        ShaderController* shaderController;
        
        void set();
        
    public:
        Buffers(Camera* camera, ShaderController* shaderController);
        ~Buffers();

        void render();
        void init();
};