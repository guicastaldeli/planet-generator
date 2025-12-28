#pragma once
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <vector>

class StarRenderer {
private:
    GLuint vao;
    GLuint vbo;
    GLuint shaderProgram;
    GLsizei starCount;
    
    struct Star {
        glm::vec3 position;
        glm::vec3 color;
        float size;
        float brightness;
        float phase;
    };
    
    std::vector<Star> stars;
    void generateStars();
    void createBuffers();

public:
    StarRenderer();
    ~StarRenderer();
    
    void init();
    void render(
        const glm::mat4& view, 
        const glm::mat4& proj, 
        float time
    );
    void cleanup();
};