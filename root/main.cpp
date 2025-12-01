#include "main.h"
#include "buffers/buffer_data.h"

EM_JS(void, setupCanvas, (int* width, int* height), {
    const canvas = document.getElementById("ctx");
    Module.canvas = canvas;
    HEAP32[width >> 2] = canvas.width;
    HEAP32[height >> 2] = canvas.height;
    //console.log("original size %d x %d", canvas.width, canvas.height);
});

int Main::initGlWindow() {
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
** Init
*/
void Main::initCamera() {
    camera = new Camera(this, shaderLoader->shaderController);
    camera->init();
}

void Main::initShaderLoader() {
    shaderLoader = new ShaderLoader();
    shaderLoader->load();
}

void Main::initBuffers() {
    buffers = new Buffers(camera, shaderLoader->shaderController, BufferData::Type::TRIANGLE);
    buffers->init();
}

void Main::initInterfaceWrapper() {
    interfaceWrapperController = new InterfaceWrapperController();
}

void Main::init() {
    ShaderLoader::setCallback([this] {
        this->initBuffers();
        this->initCamera();
    });
    initShaderLoader();
    initInterfaceWrapper();
}

/*
** Render
*/
void Main::render() {
    if(camera) camera->update();
    if(buffers) buffers->render();
}

void Main::loop() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    render();
}

int main() {
    static Main app;
    app.initGlWindow();
    app.init();
    emscripten_set_main_loop([]() {
        static Main* appPtr = &app;
        appPtr->loop();
    }, 0, 1);
    return 0;
}