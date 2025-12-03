#include "info_wrapper_controller.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <iostream>

InfoWrapperController::InfoWrapperController() {}
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
                console.log('Info appended to DOM from C++');
            }
        }, html);
    }

    void display(const char* name, const char* info) {
        EM_ASM({
            const planetName = UTF8ToString($0);
            const planetInfo = UTF8ToString($1);
                
            console.log('C++ display called:', planetName, planetInfo);
            console.log('window.display exists:', typeof window.display);
            
            if(typeof window.display === 'function') {
                window.display(planetName, planetInfo);
            } else {
                console.error('window.display function not found');
            }
        }, name, info);
    }
}