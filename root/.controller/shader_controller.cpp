#include "shader_controller.h"
#include <emscripten.h>
#include <GLES3/gl3.h>

void ShaderController::checkStatus() {
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::PROGRAM::LINKING_FAILED %s\n", infoLog);
        return;
    }
}

void ShaderController::load() {
    /* Vertex */
    std::string vertexContent = ShaderLoader::getShader(VERTEX);
    const char* vertexSrc = vertexContent.c_str();
    
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);

    /* Frag */
    std::string fragContent = ShaderLoader::getShader(FRAG); 
    const char* fragSrc = fragContent.c_str();

    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSrc, NULL);
    glCompileShader(fragShader);
}

void ShaderController::initProgram() {
    emscripten_log(EM_LOG_CONSOLE, "shader controller!");
    load();

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    checkStatus();
}