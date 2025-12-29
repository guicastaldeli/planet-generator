#include "main.h"
#include "buffers/buffer_data.h"

static Main* g_app = nullptr;

EM_JS(float, getDevicePixelRatio, (), {
    return window.devicePixelRatio || 1;
});

EM_JS(void, setupCanvas, (int* width, int* height, float* dpr), {
    const canvas = document.getElementById("ctx");
    Module.canvas = canvas;
    
    const pixelRatio = window.devicePixelRatio || 1;
    const displayWidth = Math.floor(canvas.clientWidth * pixelRatio);
    const displayHeight = Math.floor(canvas.clientHeight * pixelRatio);

    canvas.width = displayWidth;
    canvas.height = displayHeight;

    HEAP32[width >> 2] = displayWidth;
    HEAP32[height >> 2] = displayHeight;
    HEAPF32[dpr >> 2] = pixelRatio;
});

int Main::initGlWindow() {
    float dpr = 1.0f;
    setupCanvas(&width, &height, &dpr);
    if(glfwInit() != GL_TRUE) {
        printf("glfw failed!");
        return GL_FALSE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_ALPHA_BITS, 16);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);//

    GLFWwindow* window = glfwCreateWindow(
        width,
        height,
        NULL,
        NULL,
        NULL
    );
    if(!window) {
        glfwWindowHint(GLFW_SAMPLES, 4);
        window = glfwCreateWindow(width, height, "app", NULL, NULL);
        if(!window) {
            printf("Window creation failed");
            glfwTerminate();
            return GL_FALSE;
        }
    }
    glfwMakeContextCurrent(window);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, width, height);
    printf("GL context created!");
    return GL_TRUE;
}

/*
** Resize
*/
void Main::resize() {
    EM_ASM({
        const canvas = document.getElementById("ctx");
        if(canvas) {
            const dpr = window.devicePixelRatio || 1;
            const displayWidth = Math.max(1, Math.floor(canvas.clientWidth * dpr));
            const displayHeight = Math.max(1, Math.floor(canvas.clientHeight * dpr));
            
            canvas.width = displayWidth;
            canvas.height = displayHeight;

            setValue($0, displayWidth, 'i32');
            setValue($1, displayHeight, 'i32');
            console.log("HD Resize: " + displayWidth + " x " + displayHeight);
        }
    }, &width, &height);

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(camera) camera->updateProjection();
}

void Main::resizeCanvas() {
    emscripten_set_resize_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW,
        this,
        1,
        [](
            int eventType,
            const EmscriptenUiEvent* uiEvent,
            void* userData
        ) -> EM_BOOL {
            Main* app = static_cast<Main*>(userData);
            app->resize();
            return EM_TRUE;
        }
    );
}

/*
** Init
*/
void Main::init() {
    shaderLoader = new ShaderLoader();
    shaderLoader->load();
    ShaderLoader::setCallback([this] {
        shaderLoader->shaderController->initProgram();
        
        bufferController = new BufferController(this, nullptr, shaderLoader);
        bufferController->initBuffers();

        camera = new Camera(this, shaderLoader->shaderController, bufferController);
        camera->init();
        
        bufferController->setCamera(camera);
        bufferController->init();

        lightManager = new LightManager(shaderLoader->shaderController);
        lightManager->getAmbientLight()->setColor(glm::vec3(0.1333f, 0.1333f, 0.1333f));
        lightManager->getAmbientLight()->setIntensity(1.0f);
        lightManager->getAmbientLight()->setEnabled(true);

        controlsWrapperController = new ControlsWrapperController(bufferController);
        infoWapperController = new InfoWrapperController(camera, bufferController);
        generatorWrapperController = new GeneratorWrapperController(
            bufferController->
                presetManager->
                getPresetLoader(),
            bufferController
        );
    });

    resizeCanvas();
}

/*
** Render
*/
void Main::render() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    static float lastTime = 0;
    float currentTime = emscripten_get_now() / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    if(camera) camera->update();
    if(bufferController) bufferController->render(deltaTime);
    if(lightManager) lightManager->setUniforms(shaderLoader->shaderController->shaderProgram);
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
    g_app = &app;

    app.initGlWindow();
    app.init();
    emscripten_set_main_loop([]() {
        static Main* appPtr = &app;
        appPtr->loop();
    }, 0, 1);
    return 0;
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void resizeCanvas() {
        if(g_app) {
            g_app->resize();
        }
    }
}