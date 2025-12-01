#pragma once
#include "../preset/preset_loader.h"
#include "../buffers/buffer_generator.h"

class BufferController {
    private:
        PresetLoader* presetLoader;
        BufferGenerator* bufferGenerator;
        std::vector<PlanetBuffer> planetBuffers;
        PresetData currentPreset;
        
    public:
        BufferController();
        ~BufferController();

        void initPresetLoader();
        void initGenerator();
        void loadDefaultPreset();
};