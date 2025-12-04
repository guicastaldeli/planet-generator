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
    void EMSCRIPTEN_KEEPALIVE generate(const char* planetData);
    void EMSCRIPTEN_KEEPALIVE showGenerator();
#ifdef __cplusplus
}
#endif