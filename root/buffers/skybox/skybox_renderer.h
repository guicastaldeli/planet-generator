#pragma once
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include "../buffer_data.h"

class SkyboxRenderer {
    private:
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        GLuint shaderProgram;
        GLuint timeUniform;

        void createMesh();
    public:
        SkyboxRenderer();
        ~SkyboxRenderer();

        void init();
        void render(
            const glm::mat4& view, 
            const glm::mat4& proj, 
            float time
        );
        void cleanup();
};