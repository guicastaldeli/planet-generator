#include "../camera.h"
#include "buffer_controller.h";

class PreviewController {
    private:
        Camera* camera;
        BufferController* bufferController;

        void lockCamera();
        void unlockCamera();
        void positionateCamera();

    public:
        PreviewController(BufferController* bufferController, Camera* camera);
        ~PreviewController();

        void preview();
        void exitPreview();
        void init();
};  