#pragma once
#include <string>
#include "preset_data.h"

class PresetManager;
class PresetExporter {
    private:
        PresetManager* presetManager;
        const std::string fileName = "preset.json";

    public:
        PresetExporter(PresetManager* presetManager);
        ~PresetExporter();

        void exportPreset(const PresetData& preset);
        std::string getPresetData(const PresetData& preset);
};