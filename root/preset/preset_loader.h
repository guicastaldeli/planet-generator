#include "preset_data.h"

class PresetLoader {
    private:
        const std::string defaultPresetPath;
        PresetData currentPreset;

    public:
        PresetLoader(const std::string& defaultPresetPath);
        ~PresetLoader();

        bool loadPreset(const std::string& filePath);
        bool loadDefaultPreset();
        //bool PresetLoader::parseJSON(const std::string& jsonStr)
        //void PresetLoader::parseJSONPlanets(const std::string& jsonStr)
        bool PresetLoader::validatePreset();
        PresetData getCurrentPreset() const;
        PresetData& getCurrentPreset();
        void setCurrentPreset(const PresetData& preset);
};