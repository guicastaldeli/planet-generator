#pragma once
#include <emscripten/html5.h>

class PresetLoader;
class BufferController;
class GeneratorWrapperController {
    public:
        GeneratorWrapperController(
            PresetLoader* presetLoader,
            BufferController* bufferController
        );
        ~GeneratorWrapperController();

        PresetLoader* presetLoader;
        BufferController* bufferController;
};

#ifdef __cplusplus
extern "C" {
#endif
    void EMSCRIPTEN_KEEPALIVE appendGeneratorToDOM(const char* html);
    void EMSCRIPTEN_KEEPALIVE startGeneratorPreview();
    void EMSCRIPTEN_KEEPALIVE updatePreviewPlanet(const char* data);
    void EMSCRIPTEN_KEEPALIVE cleanupGeneratorPreview();
    void EMSCRIPTEN_KEEPALIVE generatePlanetParser(const char* planetData);
    void EMSCRIPTEN_KEEPALIVE showGenerator();
#ifdef __cplusplus
}
#endif