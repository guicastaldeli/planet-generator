#pragma once
#include <emscripten.h>

class InterfaceWrapperController {
    public:
        InterfaceWrapperController();
        ~InterfaceWrapperController();    
};

#ifdef __cplusplus
extern "C" {
#endif
    void EMSCRIPTEN_KEEPALIVE onControlsMenu();
    void EMSCRIPTEN_KEEPALIVE onCustomPreset();
    void EMSCRIPTEN_KEEPALIVE onImportPreset();
    void EMSCRIPTEN_KEEPALIVE onExportPreset();
    void EMSCRIPTEN_KEEPALIVE onResetToDefault();

#ifdef __cplusplus
}
#endif

