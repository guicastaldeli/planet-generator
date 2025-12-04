#pragma once
#include "../.preset/preset_loader.h"
#include "../.buffers/buffer_generator.h"
#include "../camera.h"
#include "../shader_loader.h"
#include "../.buffers/raycaster.h"
#include "../main.h"

class BufferController {
    private:
        Main* main;
        ShaderLoader* shaderLoader;
        std::vector<PlanetBuffer> planetBuffers;
        PresetData currentPreset;

        int MIN_PLANETS = 0;
        int MAX_PLANETS = 15;
        int selectedPlanetIndex;
        
    public:
        BufferController(
            Main* main, 
            Camera* Camera, 
            ShaderLoader* shaderLoader
        );
        ~BufferController();
        
        Buffers* buffers;
        Camera* camera;
        Raycaster* raycaster;
        BufferGenerator* bufferGenerator;
        PresetLoader* presetLoader;

        void setCamera(Camera* cam);
        void initBuffers();
        void initPresetLoader();
        void initGenerator();
        void init();
        void render(float deltaTime);

        void updatePlanetPositions();
        int checkPlanetIntersections(double mosueX, double mouseY);
        void handleRaycasterRender(double mouseX, double mouseY);
        void handleRaycasterClick(double mouseX, double mouseY);
        const PlanetBuffer* getSelectedPlanet() const;
        int getSelectedPlanetIndex() const;
};