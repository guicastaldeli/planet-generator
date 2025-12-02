#pragma once
#include "../preset/preset_loader.h"
#include "../buffers/buffer_generator.h"
#include "../camera.h"
#include "../shader_loader.h"

class BufferController {
    private:
        Camera* camera;
        ShaderLoader* shaderLoader;
        PresetLoader* presetLoader;
        BufferGenerator* bufferGenerator;
        std::vector<PlanetBuffer> planetBuffers;
        PresetData currentPreset;
        
    public:
        BufferController(Camera* Camera, ShaderLoader* shaderLoader);
        ~BufferController();
        Buffers* buffers;

        void initBuffers();
        void initPresetLoader();
        void initGenerator();
        void init();
        void render(float deltaTime);
};