#pragma once

class BufferController;
class PresetReset {
    private:
        BufferController* bufferController;
        
    public:
        PresetReset(BufferController* bufferController);
        ~PresetReset();

        void resetToDefault();
};