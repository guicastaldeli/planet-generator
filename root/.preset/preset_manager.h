#include "preset_loader.h"
#include "preset_save.h"
#include "preset_reset.h"
#include "preset_importer.h"
#include "preset_exporter.h"

class BufferController;
class PresetManager {
    private:
        BufferController* bufferController;
        
        PresetLoader* presetLoader;
        PresetSave* presetSave;
        PresetImporter* presetImporter;
        PresetExporter* presetExporter;
        PresetReset* presetReset;

    public:
        PresetManager(
            BufferController* bufferController
        );
        ~PresetManager();

        PresetLoader* getPresetLoader();
        PresetSave* getPresetSave();
        PresetImporter* getPresetImporter();
        PresetExporter* getPresetExporter();
        PresetReset* getPresetReset();
};