#include "interface_wrapper_controller.h"
#include <iostream>

InterfaceWrapperController::InterfaceWrapperController() {}
InterfaceWrapperController::~InterfaceWrapperController() {}

extern "C" {
    void appendToDOM(const char* html) {
        EM_ASM({
            try {
                const htmlString = UTF8ToString($0);
                
                const container = document.createElement('div');
                container.innerHTML = htmlString;
                
                const controlsContainer = container.querySelector('.controls--container');
                if(controlsContainer) {                    
                    document.body.appendChild(controlsContainer);
                    console.log('Controls appended to DOM from C++');
                }
            } catch(err) {
                console.error('Error appending controls:', err);
            }
        }, html);
    }
}

void onCustomPreset() {
    std::cout << "Custom button clicked from TypeScript!" << std::endl;
}

void onImportPreset() {
    std::cout << "Import button clicked from TypeScript!" << std::endl;
}

void onExportPreset() {
    std::cout << "Export button clicked from TypeScript!" << std::endl;
}

void onResetToDefault() {
    std::cout << "Reset button clicked from TypeScript!" << std::endl;
}

void onControlsMenu() {
    std::cout << "Main button clicked from TypeScript!" << std::endl;
}