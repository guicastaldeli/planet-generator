#pragma once
#include "preset_data.h"
#include <iostream>
#include <emscripten.h>

class PresetManager;
class PresetImporter {
    private:
        PresetManager* presetManager;
        std::function<void(const PresetData&)> importCallback;
        static std::string lastImportedData;

    public:
        PresetImporter(PresetManager* presetManager);
        ~PresetImporter();

        bool import(const std::string& data);
        void upload();
        void setImportCallback(std::function<void(const PresetData&)> cb);
};

#ifdef __cplusplus
extern "C" {
#endif
    void EMSCRIPTEN_KEEPALIVE onFileImported(const char* data);
#ifdef __cplusplus
}
#endif