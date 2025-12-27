#include "shader_controller.h"
#include <emscripten.h>
#include <GLES3/gl3.h>

ShaderController::ShaderController() {};
ShaderController::~ShaderController() {};

ShaderController* ShaderController::instance = nullptr;
ShaderController* ShaderController::getInstance() {
    if(!instance) {
        instance = new ShaderController();
    }
    return instance;
}

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

/**
 * Load
 */
void ShaderController::load() {
    /* Vertex */
    std::string vertexContent = ShaderLoader::getShader(ShaderLoader::getShaderPath()->VERTEX);
    const char* vertexSrc = vertexContent.c_str();
    
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);
    
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::VERTEX_SHADER::COMPILATION_FAILED\n%s\n", infoLog);
        printf("Vertex shader source:\n%s\n", vertexSrc);
    }

    /* Frag */
    std::string fragContent = ShaderLoader::getShader(ShaderLoader::getShaderPath()->FRAG); 
    const char* fragSrc = fragContent.c_str();

    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSrc, NULL);
    glCompileShader(fragShader);
    
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        printf("ERROR::FRAGMENT_SHADER::COMPILATION_FAILED\n%s\n", infoLog);
        printf("Fragment shader source:\n%s\n", fragSrc);
    }
}

/**
 * Init Program
 */
void ShaderController::initProgram() {
    emscripten_log(EM_LOG_CONSOLE, "shader controller!");
    load();

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    checkStatus();
}

GLuint ShaderController::getProgram() const {
    return shaderProgram;
}