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
    printf("All shaders loaded!\n");

    for(const auto& s : loadedData) {
        Type type = s.first;
        const std::string& content = s.second;
        
        switch(type) {
            case VERTEX:
                EM_ASM({ console.groupCollapsed("Vertex Shader") });
                EM_ASM_({ console.log(UTF8ToString($0)) }, content.c_str());
                EM_ASM({ console.groupEnd(); });
                break;
            case FRAG:
                EM_ASM({ console.groupCollapsed("Frag Shader") });
                EM_ASM_({ console.log(UTF8ToString($0)) }, content.c_str());
                EM_ASM({ console.groupEnd() });
                break;
            case COLOR:
                EM_ASM({ console.groupCollapsed("Color Shader") });
                EM_ASM_({ console.log(UTF8ToString($0)) }, content.c_str());
                EM_ASM({ console.groupEnd() });
                break;
            case TEXTURE:
                EM_ASM({ console.groupCollapsed("Texture Shader") });
                EM_ASM_({ console.log(UTF8ToString($0)) }, content.c_str());
                EM_ASM({ console.groupEnd() });
                break;
            case AMBIENT_LIGHT:
                EM_ASM({ console.groupCollapsed("Ambient Light Shader") });
                EM_ASM_({ console.log(UTF8ToString($0)) }, content.c_str());
                EM_ASM({ console.groupEnd() });
                break;
            case POINT_LIGHT:
                EM_ASM({ console.groupCollapsed("Point Light Shader") });
                EM_ASM_({ console.log(UTF8ToString($0)) }, content.c_str());
                EM_ASM({ console.groupEnd() });
                break;
            case SKYBOX:
                EM_ASM({ console.groupCollapsed("Skybox Shader") });
                EM_ASM_({ console.log(UTF8ToString($0)) }, content.c_str());
                EM_ASM({ console.groupEnd() });
                break;
            case FRESNEL:
                EM_ASM({ console.groupCollapsed("Fresnel Shader") });
                EM_ASM_({ console.log(UTF8ToString($0)) }, content.c_str());
                EM_ASM({ console.groupEnd() });
                break;
            case NOISE:
                EM_ASM({ console.groupCollapsed("Noise Shader") });
                EM_ASM_({ console.log(UTF8ToString($0)) }, content.c_str());
                EM_ASM({ console.groupEnd() });
                break;
        }
    }
    if(dataCallback) {
        auto callback = dataCallback;
        dataCallback = nullptr;
        callback();
    }
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

const std::string& ShaderLoader::getShader(Type type) {
    return loadedData[type];
}