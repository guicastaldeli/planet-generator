#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../buffer_data.h"

class SkyboxRenderer {
    private:
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        GLuint shaderProgram;

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
}