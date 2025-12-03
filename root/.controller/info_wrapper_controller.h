#pragma once
#include <emscripten/html5.h>

class InfoWrapperController {
    public:
        InfoWrapperController();
        ~InfoWrapperController();
};

#ifdef __cplusplus
extern "C" {
#endif
    void EMSCRIPTEN_KEEPALIVE appendInfoToDOM(const char* html);
    void EMSCRIPTEN_KEEPALIVE display(const char* name, const char* info);
#ifdef __cplusplus
}
#endif