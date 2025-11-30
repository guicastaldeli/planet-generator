#include "shader_controller.h"
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
    /* Vertex Shader */
    std::string vertexContent = ShaderLoader::getShader(VERTEX);
    const char* vertexSrc = vertexContent.c_str();
    
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);
    
    // Check vertex shader compilation
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::VERTEX::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
        printf("Vertex shader source:\n%s\n", vertexSrc);
    }

    /* Fragment Shader */
    std::string fragContent = ShaderLoader::getShader(FRAG); 
    const char* fragSrc = fragContent.c_str();

    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSrc, NULL);
    glCompileShader(fragShader);
    
    // Check fragment shader compilation
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        printf("ERROR::FRAGMENT::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
        printf("Fragment shader source:\n%s\n", fragSrc);
    }
}
void ShaderController::initProgram() {
    load();

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    checkStatus();
}