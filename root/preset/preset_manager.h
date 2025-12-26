#pragma once
#include "preset_loader.h"
#include "preset_saver.h"
#include "preset_reset.h"
#include "preset_importer.h"
#include "preset_exporter.h"

class BufferController;
class PresetManager {
    private:
        BufferController* bufferController;
        
        PresetLoader* presetLoader;
        PresetSaver* presetSaver;
        PresetImporter* presetImporter;
        PresetExporter* presetExporter;
        PresetReset* presetReset;

    public:
        PresetManager(
            BufferController* bufferController
        );
        ~PresetManager();

        PresetLoader* getPresetLoader();
        PresetSaver* getPresetSaver();
        PresetImporter* getPresetImporter();
        PresetExporter* getPresetExporter();
        PresetReset* getPresetReset();
};