#include "preset_importer.h"
#include "preset_manager.h"
#include "preset_saver.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <iostream>
#include <sstream>
#include <fstream>

PresetManager* g_presetManager = nullptr;

PresetImporter::PresetImporter(PresetManager* presetManager) :
    presetManager(presetManager),
    importCallback(nullptr)
{
    g_presetManager = presetManager;
};
PresetImporter::~PresetImporter() {};

/*
** Upload
*/
void PresetImporter::upload() {
    EM_ASM({
        const input = document.createElement('input');
        input.type = 'file';
        input.accept = '.json,application/json';
        input.style.display = 'none';

        input.addEventListener('change', function(ev) {
            const file = ev.target.files[0];
            if(!file) return;

            const reader = new FileReader();
            reader.onload = function(e) {
                try {
                    const content = e.target.result;
                    console.log('file loaded!');

                    const contentLength = lengthBytesUTF8(content) + 1;
                    const dataPtr = _malloc(contentLength);
                    stringToUTF8(content, dataPtr, contentLength);
                    Module._onFileImported(dataPtr);
                    _free(dataPtr);
                } catch(err) {
                    console.error('Error reading file:', err);
                }
            };
            reader.onerror = function(e) {
                console.error('File reader err!', e);
            };
            reader.readAsText(file);
        });

        document.body.appendChild(input);
        input.click();

        setTimeout(() => {
            document.body.removeChild(input);
        }, 100);
    });
}

/*
** Import Preset
*/
bool PresetImporter::import(const std::string& data) {
    if(!presetManager || !presetManager->getPresetSaver()) {
        std::cerr << "Preset manager or saver not available" << std::endl;
        return false;
    }
    if(data.empty()) {
        std::cerr << "Empty data provided!" << std::endl;
        return false;
    }

    try {
        PresetData preset;
        bool success = presetManager->
            getPresetSaver()->
            convertToPreset(data, preset);
        if(!success) {
            std::cerr << "Failed to parse data!" << std::endl;
            return false;
        }
        if(preset.planets.empty()) {
            std::cerr << "No planets!" << std::endl;
            return false;
        }

        std::cout << "Successfully imported preset with " 
                  << preset.planets.size() << " planets" << std::endl;

        if(importCallback) importCallback(preset);
        return true;
    } catch(const std::exception& err) {
        std::cerr << "Err!: " << err.what() << std::endl;
        return false;
    }
}

void PresetImporter::setImportCallback(std::function<void(const PresetData&)> cb) {
    importCallback = cb;
}

extern "C" {
    void onFileImported(const char* data) {
        std::cout << "Import button clicked from TypeScript!" << std::endl;
        if(g_presetManager) {
            PresetImporter* importer = g_presetManager->getPresetImporter();
            if(importer && data) {
                std::string presetData(data);
                importer->import(presetData);
            }
        }
    }
}