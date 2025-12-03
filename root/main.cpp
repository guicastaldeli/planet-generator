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
void Main::init() {
    shaderLoader = new ShaderLoader();
    shaderLoader->load();
    ShaderLoader::setCallback([this] {
        bufferController = new BufferController(this, nullptr, shaderLoader);
        bufferController->init();
        camera = new Camera(this, shaderLoader->shaderController, bufferController);
        camera->init();
        bufferController->setCamera(camera);
    });
    
    interfaceWrapperController = new InterfaceWrapperController();
    infoWapperController = new InfoWrapperController();
}

/*
** Render
*/
void Main::render() {
    static float lastTime = 0;
    float currentTime = emscripten_get_now() / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    /*
    emscripten_console_log(std::to_string(deltaTime).c_str());
    emscripten_console_log(std::to_string(lastTime).c_str());
    emscripten_console_log(std::to_string(currentTime).c_str());
    */

    if(camera) camera->update();
    if(bufferController) bufferController->render(deltaTime);
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