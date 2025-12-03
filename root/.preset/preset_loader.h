#pragma once
#include "preset_data.h"

class PresetLoader {
    private:
        const std::string defaultPresetPath;
        PresetData currentPreset;

    public:
        PresetLoader(const std::string& defaultPresetPath);
        ~PresetLoader();

        bool parse(const std::string& data);
        bool loadPreset(const std::string& filePath);
        bool loadDefaultPreset();
        bool validatePreset();
        PresetData getCurrentPreset() const;
        PresetData& getCurrentPreset();
        void setCurrentPreset(const PresetData& preset);
};