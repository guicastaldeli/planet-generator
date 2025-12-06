#include "preset_exporter.h"
#include "preset_manager.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <iostream>
#include <sstream>

PresetExporter::PresetExporter(PresetManager* presetManager) :
    presetManager(presetManager),
    fileName("preset.json") 
{};
PresetExporter::~PresetExporter() {};

/*
** Get Preset Data
*/
std::string PresetExporter::getPresetData(const PresetData& preset) {
    if(!presetManager || !presetManager->getPresetSaver()) {
        std::cerr << "Preset saver not available" << std::endl;
        return "";
    }

    return presetManager->
        getPresetSaver()->
        presetToJson(const_cast<PresetData&>(preset));
}

/*
** Export Preset
*/
void PresetExporter::exportPreset(const PresetData& preset) {
    std::string data = getPresetData(preset);
    if(data.empty()) {
        std::cerr << "Failed to convert to JSON!" << std::endl;
        return;
    }

    const char* dataStr = data.c_str();
    const char* fileNameStr = fileName.c_str();

    EM_ASM_INT({
        try {
            const data = UTF8ToString($0);
            const fileName = UTF8ToString($1);

            const blob = new Blob(
                [data],
                { type: 'application/json' }
            );
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = fileName;

            document.body.appendChild(a);
            a.click();

            setTimeout(() => {
                document.body.removeChild(a);
                URL.revokeObjectURL(url);
            });
            console.log('Preset exported!', fileName);
            return 1;
        } catch(err) {
            console.error('Error exporting preset!', err);
            return 0;
        }
    }, dataStr, fileNameStr);
}