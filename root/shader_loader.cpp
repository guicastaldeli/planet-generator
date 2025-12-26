#include "shader_loader.h"
#include <fstream>
#include <string>
#include <cstring>
#include <emscripten/fetch.h>
#include <emscripten/emscripten.h>
#include ".controller/shader_controller.h"

ShaderLoader::ShaderLoader() {
    shaderController = new ShaderController();
}
ShaderLoader::~ShaderLoader() {}

std::unordered_map<Type, std::string> ShaderLoader::loadedData;
std::function<void()> ShaderLoader::dataCallback = nullptr;
std::vector<Request> ShaderLoader::request;
int ShaderLoader::pendingLoads = 0;

void ShaderLoader::onDataLoaded() {
    printf("\n=== ALL SHADERS LOADED ===\n");
}

void ShaderLoader::setCallback(std::function<void()> callback) {
    dataCallback = callback;
}

void ShaderLoader::addUrl(const std::string& url, Type type) {
    request.push_back({ url, type });
}

void ShaderLoader::onSuccess(emscripten_fetch_t * fetch) {
    for(const auto& r : request) {
        if(r.url == fetch->url) {
            loadedData[r.type] = std::string(fetch->data, fetch->numBytes);
            break;
        }
    }
    emscripten_fetch_close(fetch);
    pendingLoads--;
    if(pendingLoads == 0) onDataLoaded();
}

void ShaderLoader::onError(emscripten_fetch_t *fetch) {
    printf("Failed to load file!: %s\n", fetch->url);
    emscripten_fetch_close(fetch);
    pendingLoads--;
    if(pendingLoads == 0) onDataLoaded();
}

void ShaderLoader::load() {
    loadedData.clear();
    request.clear();

    for(const auto& file : files) {
        std::string path = "_shaders/" + file.fileName;
        addUrl(path, file.type);
    }
    pendingLoads = request.size();
    printf("\nStarting to load %d shaders...\n", pendingLoads);

    for(const auto& r : request) {
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, "GET");

        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = ShaderLoader::onSuccess;
        attr.onerror = ShaderLoader::onError;

        emscripten_fetch(&attr, r.url.c_str());
    }
}

/**
 * Set Shader
 */
std::string ShaderLoader::getShader(Type type, const std::vector<Type>& funcTypes) {
    if(loadedData.find(type) == loadedData.end()) {
        printf("Warning: Main shader type %d not found\n", type);
        return "";
    }

    std::string mainShader = loadedData[type];
    size_t mainPos = mainShader.find("void main()");
    
    std::vector<Type> allModules = funcTypes;
    if(mainPos == std::string::npos) return mainShader + concatModules(allModules);

    std::string before = mainShader.substr(0, mainPos);
    std::string after = mainShader.substr(mainPos);

    return before + concatModules(allModules) + after + concatModules(allModules);
}

/**
 * Concat Modules
 */
std::string ShaderLoader::concatModules(const std::vector<Type>& funcTypes) {
    std::string modules;
    for(Type t : funcTypes) {
        auto it = loadedData.find(t);
        if(it != loadedData.end()) {
            modules += "\n\n// ================ " + std::to_string(t) + " MODULE ================\n";
            modules += it->second;
        } else {
            printf("Warning: Module type %d not found\n", t);
        }
    }
    return modules;
}