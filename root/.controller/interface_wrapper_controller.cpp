#include "interface_wrapper_controller.h"
#include <iostream>

InterfaceWrapperController::InterfaceWrapperController() {}
InterfaceWrapperController::~InterfaceWrapperController() {}

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