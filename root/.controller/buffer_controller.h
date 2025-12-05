#pragma once
#include "../.preset/preset_loader.h"
#include "../.buffers/buffer_generator.h"
#include "../camera.h"
#include "../shader_loader.h"
#include "../.buffers/raycaster.h"
#include "../main.h"

class PreviewController;
class BufferController {
    private:
        Main* main;
        ShaderLoader* shaderLoader;
        std::vector<PlanetBuffer> planetBuffers;
        PresetData currentPreset;

        int MIN_PLANETS = 0;
        int MAX_PLANETS = 15;
        int selectedPlanetIndex;
        bool presetLoaded;
        
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
        PreviewController* previewController;

        void initBuffers();
        void initPresetLoader();
        void initGenerator();
        void initPreviewController();

        void setCamera(Camera* cam);
        void init();
        void render(float deltaTime);

        void clearBuffers();
        void updatePlanetPositions();
        int checkPlanetIntersections(double mosueX, double mouseY);
        void handleRaycasterRender(double mouseX, double mouseY);
        void handleRaycasterClick(double mouseX, double mouseY);
        const PlanetBuffer* getSelectedPlanet() const;
        int getSelectedPlanetIndex() const;
        void deleteSelectedPlanet();
};