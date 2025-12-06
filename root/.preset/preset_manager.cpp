#include "preset_manager.h"

PresetManager::PresetManager(BufferController* bufferController) :
    bufferController(bufferController)
{
    presetLoader = new PresetLoader();
    presetSave = new PresetSave();
    presetImporter = new PresetImporter();
    presetExporter = new PresetExporter();
    presetReset = new PresetReset(bufferController);
}
PresetManager::~PresetManager() {};

/*
** Preset Loader
*/
PresetLoader* PresetManager::getPresetLoader() {
    return presetLoader;
}

/*
** Preset Save
*/
PresetSave* PresetManager::getPresetSave() {
    return presetSave;
}

/*
** Preset Importer
*/
PresetImporter* PresetManager::getPresetImporter() {
    return presetImporter;
}

/*
** Preset Exporter
*/
PresetExporter* PresetManager::getPresetExporter() {
    return presetExporter;
}

/*
** Preset Reset
*/
PresetReset* PresetManager::getPresetReset() {
    return presetReset;
}
