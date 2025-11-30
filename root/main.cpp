#include "main.h"

EM_JS(void, setupCanvas, (int* width, int* height), {
    const canvas = document.getElementById("ctx");
    Module.canvas = canvas;
    HEAP32[width >> 2] = canvas.width;
    HEAP32[height >> 2] = canvas.height;
    //console.log("original size %d x %d", canvas.width, canvas.height);
});

int Main::initGlWindow() {
    int width;
    int height;
    setupCanvas(&width, &height);
    printf("Canvas size: %d x %d\n", width, height);
    if(glfwInit() != GL_TRUE) {
        printf("gl failed!");
        return GL_FALSE;
    }

    GLFWwindow* window = glfwCreateWindow(
        width,
        height,
        NULL,
        NULL,
        NULL
    );
    if(!window) {
        printf("Window creation failed");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    printf("GL context created!");
    return GL_TRUE;
}

/*
** Init Shader Loader
*/
void Main::initShaderLoader() {
    ShaderLoader::setCallback(ShaderLoader::onDataLoaded);
    ShaderLoader shaderLoader;
    shaderLoader.load();
}

/*
** Init Buffers
*/
void Main::initBuffers() {
    Buffers buffers;
    buffers.init();
}

/*
** Main Render
*/
void Main::render() {
    ShaderLoader shaderLoader;
    shaderLoader.setCallback([] {
        initBuffers();
    });
    initShaderLoader();
}

int main() {
    initGlWindow();
    render();
    emscripten_set_main_loop([](){}, 0, 1);
    return 0;
}