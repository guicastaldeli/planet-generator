#include "preset_manager.h"

PresetManager::PresetManager(BufferController* bufferController) :
    bufferController(bufferController)
{
    presetLoader = new PresetLoader(this);
    presetSaver = new PresetSaver(bufferController);
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
PresetSaver* PresetManager::getPresetSaver() {
    return presetSaver;
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
