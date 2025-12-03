#pragma once
#include <emscripten/html5.h>

class GeneratorWrapperController {
    public:
        GeneratorWrapperController();
        ~GeneratorWrapperController();
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