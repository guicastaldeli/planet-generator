#pragma once
#include "../camera.h"
#include "buffer_controller.h"
#include "../lightning/light_manager.h"

class PreviewController {
    private:
        Camera* camera;
        BufferController* bufferController;
        LightManager* lightManager;

        static const int PREVIEW_LIGHT_ID = -999999;
        int previewLightId;

        void lockCamera();
        void unlockCamera();

    public:
        PreviewController(
            BufferController* bufferController, 
            Camera* camera,
            LightManager* lightManager
        );
        ~PreviewController();

        bool isPreviewing;
        bool isGeneratorActive;

        void preview();
        void exitPreview();

        void startGeneratorPreview();
        void updatePreview(const PlanetData& data);
        void cleanupPreview();
        bool isInGeneratorMode() const;

        void createPreviewLight();
        void removePreviewLight();
};  