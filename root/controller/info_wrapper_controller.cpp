#include "info_wrapper_controller.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <iostream>
#include "../camera.h"
#include "buffer_controller.h"

InfoWrapperController* g_infoWrapperController = nullptr;

InfoWrapperController::InfoWrapperController(Camera* camera, BufferController* bufferController) :
    camera(camera),
    bufferController(bufferController) 
{
    g_infoWrapperController = this;
}
InfoWrapperController::~InfoWrapperController() {}

extern "C" {
    void appendInfoToDOM(const char* html) {
        EM_ASM({
            const htmlString = UTF8ToString($0);
            
            const container = document.createElement('div');
            container.innerHTML = htmlString;
            
            const infoContainer = container.querySelector('.info--container');
            if(infoContainer) {                    
                document.body.appendChild(infoContainer);
            }
        }, html);
    }

    /*
     * Display Info
     */
    void display(const char* name, const char* info) {
        EM_ASM({
            const planetName = UTF8ToString($0);
            const planetInfo = UTF8ToString($1);
            
            if(typeof window.display === 'function') {
                window.display(planetName, planetInfo);
            } else {
                console.error('window display function not found');
            }
        }, name, info);
    }

    /*
     * Close
     */
    void closeMenu() {
        if(g_infoWrapperController && g_infoWrapperController->camera) {
            g_infoWrapperController->camera->resetToSavedPos();
        }
        EM_ASM({
            const container = document.querySelector('.info--container');
            if(container) {
                container.style.display = 'none';
            }
        });
    }

    /*
     * Delete Planet
     */
    void deletePlanet() {
        if(g_infoWrapperController && g_infoWrapperController->bufferController) {
            g_infoWrapperController->bufferController->deleteSelectedPlanet();
        }
        EM_ASM({
            const container = document.querySelector('.info--container');
            if(container) {
                container.style.display = 'none';
            } 
        });
    }
}