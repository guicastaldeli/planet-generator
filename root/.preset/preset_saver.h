#pragma once
#include "preset_data.h"
#include "../_data/data_parser.h"

class BufferController;
class PresetManager;
class PresetSaver {
    private:
        BufferController* bufferController;
        PresetManager* presetManager;
        
        const std::string key = "savedPreset";

        DataParser::Value presetToVal(PresetData& preset);
        bool valueToPreset(const DataParser::Value& val, PresetData& preset);
        DataParser::Value planetToValue(const PlanetData& planet);
        bool valueToPlanet(const DataParser::Value& val, PlanetData& planet);

    public:
        PresetSaver(BufferController* bufferController, PresetManager* presetManager);
        ~PresetSaver();

        bool saveToLocalStorage(PresetData& preset);
        bool loadFromLocalStorage(PresetData& preset);
        bool hasSavedPreset();
        void clearLocalStorage();

        std::string presetToJson(PresetData& preset);
        bool jsonToPreset(const std::string& data, PresetData& preset);

        bool save();
        bool load();
};