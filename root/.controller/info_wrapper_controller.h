#pragma once
#include <emscripten/html5.h>

class Camera;
class BufferController;
class InfoWrapperController {
    public:
        InfoWrapperController(Camera* camera, BufferController* bufferController);
        ~InfoWrapperController();

        Camera* camera;
        BufferController* bufferController;
};

#ifdef __cplusplus
extern "C" {
#endif
    void EMSCRIPTEN_KEEPALIVE appendInfoToDOM(const char* html);
    void EMSCRIPTEN_KEEPALIVE closeMenu();
    void EMSCRIPTEN_KEEPALIVE deletePlanet();
    void EMSCRIPTEN_KEEPALIVE display(const char* name, const char* info);
#ifdef __cplusplus
}
#endif