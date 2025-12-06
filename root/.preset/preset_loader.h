#pragma once
#include "preset_data.h"

class PresetManager;
class PresetLoader {
    private:
        PresetManager* presetManager;
        std::string defaultPresetPath;
        PresetData currentPreset;

    public:
        PresetLoader(PresetManager* presetManager);
        ~PresetLoader();

        void setPath(std::string& path);
        bool parse(const std::string& data);
        bool loadPreset(const std::string& filePath);
        bool loadDefaultPreset();
        bool validatePreset();
        PresetData getCurrentPreset() const;
        PresetData& getCurrentPreset();
        void setCurrentPreset(const PresetData& preset);
};