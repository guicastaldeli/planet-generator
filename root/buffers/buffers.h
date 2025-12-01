#pragma once
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include "buffer_types.h"

class Camera;
class ShaderController;
class Buffers {
    private:
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        BufferTypes::Type bufferType;
        GLuint indexCount;
        
        Camera* camera;
        ShaderController* shaderController;
        
        void set();
        
    public:
        Buffers(
            Camera* camera, 
            ShaderController* shaderController,
            BufferTypes::Type type
        );
        ~Buffers();

        void render();
        void init();
};