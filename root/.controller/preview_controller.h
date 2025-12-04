#include "../camera.h"
#include "buffer_controller.h";

class PreviewController {
    private:
        Camera* camera;
        BufferController* bufferController;

    public:
        PreviewController(BufferController* bufferController, Camera* camera);
        ~PreviewController();

        void lockCamera();
        void positionateCamera();
        void previewPlanet();

        void init();
};  