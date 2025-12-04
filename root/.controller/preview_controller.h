#pragma once
#include "../camera.h"
#include "buffer_controller.h";

class PreviewController {
    private:
        Camera* camera;
        BufferController* bufferController;

        bool isPreviewing;
        bool isGeneratorActive;

        void lockCamera();
        void unlockCamera();
        void positionateCamera();

    public:
        PreviewController(BufferController* bufferController, Camera* camera);
        ~PreviewController();

        void preview();
        void exitPreview();

        void startGeneratorPreview();
        void updatePreview(const PlanetData& data);
        void cleanupPreview();
        bool isInGeneratorMode() const { return isGeneratorActive; }
        void init();
};  