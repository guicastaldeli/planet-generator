#pragma once
#include <emscripten.h>

class BufferController;
class ControlsWrapperController {
    private:
        BufferController* bufferController;
        
    public:
        ControlsWrapperController(BufferController* bufferController);
        ~ControlsWrapperController();    
};

#ifdef __cplusplus
extern "C" {
#endif
    void EMSCRIPTEN_KEEPALIVE onControlsMenu();
    void EMSCRIPTEN_KEEPALIVE onCustomPreset();
    void EMSCRIPTEN_KEEPALIVE onSavePreset();
    void EMSCRIPTEN_KEEPALIVE onImportPreset();
    void EMSCRIPTEN_KEEPALIVE onExportPreset();
    void EMSCRIPTEN_KEEPALIVE onResetToDefault();
    void EMSCRIPTEN_KEEPALIVE onClear();
    void EMSCRIPTEN_KEEPALIVE appendToDOM(const char* html);
#ifdef __cplusplus
}
#endif

