#include "controls_wrapper_controller.h"
#include <iostream>

ControlsWrapperController::ControlsWrapperController() {}
ControlsWrapperController::~ControlsWrapperController() {}

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

    /*
     * Controls Menu
     */
    void onControlsMenu() {
        std::cout << "Main button clicked from TypeScript!" << std::endl;
    }
    
    /*
     * Custom Preset
     */
    void onCustomPreset() {
        std::cout << "Custom button clicked from TypeScript!" << std::endl;
    
        EM_ASM({
            (function() {
                if(typeof window._showGenerator !== 'undefined') {
                    console.log('onCustomPreset call');
                    window._showGenerator();
                } else {
                    console.error('showGenerator not found!');
                }
            })();
        });
    }
    
    /*
     * Import Preset
     */
    void onImportPreset() {
        std::cout << "Import button clicked from TypeScript!" << std::endl;
    }
    
    /*
     * Export Preset
     */
    void onExportPreset() {
        std::cout << "Export button clicked from TypeScript!" << std::endl;
    }
    
    /*
     * Reset to Default Preset
     */
    void onResetToDefault() {
        std::cout << "Reset button clicked from TypeScript!" << std::endl;
    }

    /*
     * Clear Preset
     */
    void onClear() {
        std::cout << "Clear button clicked from TypeScript!" << std::endl;
    }
}
