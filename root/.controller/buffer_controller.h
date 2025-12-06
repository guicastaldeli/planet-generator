#pragma once
#include "../.preset/preset_manager.h"
#include "../.buffers/buffer_generator.h"
#include "../camera.h"
#include "../shader_loader.h"
#include "../.buffers/raycaster.h"
#include "../main.h"

class PreviewController;
class BufferController {
    public:
        Main* main;
        ShaderLoader* shaderLoader;
        std::vector<PlanetBuffer> planetBuffers;
        PresetData currentPreset;

        int MIN_PLANETS = 0;
        int MAX_PLANETS = 15;
        int selectedPlanetIndex;
        bool presetLoaded;
        
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
        PresetManager* presetManager;
        PreviewController* previewController;

        void initBuffers();
        void initPresetManager();
        void setPresetPath();
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