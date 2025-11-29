#include "shader_loader.h"
#include <fstream>
#include <string>
#include <cstring>
#include <emscripten/fetch.h>

std::string ShaderLoader::loadedData = "";
std::function<void(const std::string&)> ShaderLoader::dataCallback = nullptr;
void ShaderLoader::onDataLoaded(const std::string& data) {
    printf("Data loaded!: %s\n", data.c_str());
}
void ShaderLoader::setCallback(std::function<void(const std::string&)> callback) {
    dataCallback = callback;
}

void ShaderLoader::onSuccess(emscripten_fetch_t * fetch) {
    printf("Fetched %llu bytes\n", fetch->numBytes);
    loadedData = std::string(fetch->data, fetch->numBytes);
    emscripten_fetch_close(fetch);
    if(dataCallback) dataCallback(loadedData);
}

void ShaderLoader::onError(emscripten_fetch_t *fetch) {
    printf("Failed to load file!\n");
    loadedData = "";
    emscripten_fetch_close(fetch);
    if(dataCallback) dataCallback("");
}

void ShaderLoader::load() {
    loadedData = "";

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");

    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = ShaderLoader::onSuccess;
    attr.onerror = ShaderLoader::onError;

    emscripten_fetch(&attr, "test.txt");
}