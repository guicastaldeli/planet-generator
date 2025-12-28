#include "star_renderer.h"
#include "../../_shaders/shader_controller.h"
#include <random>
#include <iostream>
#include <glm/gtc/constants.hpp>
#include <emscripten/html5.h>

StarRenderer::StarRenderer() :
    vao(0),
    vbo(0),
    shaderProgram(0),
    starCount(1000)
{};
StarRenderer::~StarRenderer() {
    cleanup();
}

void StarRenderer::init() {
    ShaderController* shaderController = ShaderController::getInstance();
    if(shaderController) {
        shaderProgram = shaderController->getProgram();
        printf("StarRenderer: shaderProgram = %u\n", shaderProgram);
    } else {
        printf("StarRenderer: ShaderController is NULL!\n");
    }
    
    generateStars();
    createBuffers();
    
    printf("StarRenderer: Generated %d stars\n", starCount);
}

/**
 * Generate Stars
 */
void StarRenderer::generateStars() {
    stars.clear();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distPos(-1.0f, 1.0f);
    std::uniform_real_distribution<float> distBright(0.5f, 1.0f);
    std::uniform_real_distribution<float> distSize(0.1f, 0.4f);

    for(int i = 0; i < starCount; i++) {
        Star star;

        float theta = distPos(gen) * glm::pi<float>();
        float phi = distPos(gen) * 2.0f * glm::pi<float>();

        float radius = 50.0f;
        star.position.x = radius * sin(theta) * cos(phi);
        star.position.y = radius * sin(theta) * sin(phi);
        star.position.z = radius * cos(theta);
        
        float colorVar = distBright(gen) * 0.2f;
        star.color = glm::vec3(1.0f, 1.0f - colorVar, 1.0f - colorVar * 0.5f);

        star.size = distSize(gen);
        star.brightness = distBright(gen);

        stars.push_back(star);
    }
}

/**
 * Create Buffers
 */
void StarRenderer::createBuffers() {
    std::vector<float> vertexData;
    for(const auto& star : stars) {
        vertexData.push_back(star.position.x);
        vertexData.push_back(star.position.y);
        vertexData.push_back(star.position.z);

        vertexData.push_back(star.color.r);
        vertexData.push_back(star.color.g);
        vertexData.push_back(star.color.b);

        vertexData.push_back(star.size);
        vertexData.push_back(star.brightness);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertexData.size() * sizeof(float),
        vertexData.data(),
        GL_STATIC_DRAW
    );

    GLint posLoc = glGetAttribLocation(shaderProgram, "aPos");
    GLint colorLoc = glGetAttribLocation(shaderProgram, "aColor");
    GLint attrLoc = glGetAttribLocation(shaderProgram, "starAttr");
    
    printf("StarRenderer attribute locations: aPos=%d, aColor=%d, starAttr=%d\n", 
           posLoc, colorLoc, attrLoc);
    printf("First star: pos=(%.2f, %.2f, %.2f), color=(%.2f, %.2f, %.2f), size=%.2f, brightness=%.2f\n",
           stars[0].position.x, stars[0].position.y, stars[0].position.z,
           stars[0].color.r, stars[0].color.g, stars[0].color.b,
           stars[0].size, stars[0].brightness);
    
    if(posLoc != -1) {
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(posLoc);
    }
    if(colorLoc != -1) {
        glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(colorLoc);
    }
    if(attrLoc != -1) {
        glVertexAttribPointer(attrLoc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(attrLoc);
    }

    glBindVertexArray(0);
}

/**
 * Render
 */
void StarRenderer::render(
    const glm::mat4& view,
    const glm::mat4& proj,
    float time
) {
    if(!shaderProgram || starCount == 0) return;

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);

    GLuint shaderTypeLoc = glGetUniformLocation(shaderProgram, "shaderType");
    if(shaderTypeLoc != -1) glUniform1f(shaderTypeLoc, 2.0f);

    glm::mat4 model = glm::mat4(1.0f);
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    if(modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint timeLoc = glGetUniformLocation(shaderProgram, "uTime");
    if(viewLoc != -1) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    if(projLoc != -1) glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);
    if(timeLoc != -1) glUniform1f(timeLoc, time);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_POINTS, 0, starCount);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glBindVertexArray(0);
}

void StarRenderer::cleanup() {
    if(vao) glDeleteVertexArrays(1, &vao);
    if(vbo) glDeleteBuffers(1, &vbo);
}