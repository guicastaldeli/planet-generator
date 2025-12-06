#pragma once
#include "../camera.h"
#include "buffer_controller.h"

class PreviewController {
    private:
        Camera* camera;
        BufferController* bufferController;

        void lockCamera();
        void unlockCamera();

    public:
        PreviewController(BufferController* bufferController, Camera* camera);
        ~PreviewController();

        bool isPreviewing;
        bool isGeneratorActive;

        void preview();
        void exitPreview();

        void startGeneratorPreview();
        void updatePreview(const PlanetData& data);
        void cleanupPreview();
        bool isInGeneratorMode() const { 
            return isGeneratorActive;
        }
};  