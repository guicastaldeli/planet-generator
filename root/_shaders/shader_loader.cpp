#include "shader_loader.h"
#include <fstream>
#include <string>
#include <cstring>
#include <emscripten/fetch.h>
#include <emscripten/emscripten.h>
#include "shader_controller.h"

std::string path = "/root/_shaders/";
std::vector<File> ShaderLoader::files = {
    { path + "main/vertex.glsl", VERTEX, VERTEX_SHADER_TYPE },
    { path + "main/frag.glsl", FRAG, FRAG_SHADER_TYPE},
    { path + "main/color.glsl", COLOR, VERTEX_SHADER_TYPE },
    { path + "main/texture.glsl", TEXTURE, FRAG_SHADER_TYPE },
    { path + "lightning/ambient_light.glsl", AMBIENT_LIGHT, FRAG_SHADER_TYPE },
    { path + "lightning/point_light.glsl", POINT_LIGHT, FRAG_SHADER_TYPE },
    { path + "skybox/skybox.glsl", SKYBOX, FRAG_SHADER_TYPE },
    { path + "effect/fresnel.glsl", FRESNEL, FRAG_SHADER_TYPE },
    { path + "effect/noise.glsl", NOISE, FRAG_SHADER_TYPE }
};
ShaderLoader::ShaderLoader() {
    shaderController = new ShaderController();
}
ShaderLoader::~ShaderLoader() {}

std::unordered_map<Type, std::string> ShaderLoader::loadedData;
std::function<void()> ShaderLoader::dataCallback = nullptr;
std::vector<Request> ShaderLoader::request;
int ShaderLoader::pendingLoads = 0;

void ShaderLoader::onDataLoaded() {
    if(dataCallback) dataCallback();
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

/**
 * Process Includes
 */
std::string ShaderLoader::processIncudes(const std::string& content, const std::string& parentFile) {
    std::string result;
    std::stringstream ss(content);
    std::string line;
    std::string parentDir = getParentDir(parentFile);
    while(std::getline(ss, line)) {
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
        if(trimmed.find("#include ") == 0) {
            std::string file = trimmed.substr(9);
            file.erase(file.find_first_not_of(" \t\"'"));
            file.erase(file.find_last_not_of(" \t\"'") + 1);

            std::string path;
            if(file.find('/') != std::string::npos) {
                path = file;
            } else {
                path = parentDir + file;
            }

            std::string includeContent = loadFile(path);
            includeContent = processIncudes(includeContent, path);
            includeContent = stripVersionDir(includeContent);
            result += includeContent + "\n";
        } else {
            result += line + "\n";
        }
    }
    return result;
}

std::string ShaderLoader::getParentDir(const std::string& path) {
    size_t lastSlash = path.find_last_of('/');
    if(lastSlash != std::string::npos) {
        return path.substr(0, lastSlash + 1);
    }
    return "";
}

std::string ShaderLoader::stripVersionDir(const std::string& content) {
    std::stringstream ss(content);
    std::string result;
    std::string line;
    bool versionFound = false;

    while(std::getline(ss, line)) {
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        if(trimmed.find("#version") == 0) {
            if(!versionFound) {
                result += line + "\n";
                versionFound = true;
            }
        } else {
            result += line + "\n";
        }
    }
    return result;
}

/**
 * Set Shader
 */
std::string ShaderLoader::getShader(Type type) {
    if(loadedData.find(type) == loadedData.end()) {
        printf("Main shader type %d not found\n", type);
        return "";
    }

    std::string mainShader = loadedData[type];
    
    bool isVertexShader = false;
    for(const auto& file : files) {
        if(file.type == type) {
            isVertexShader = (file.shaderType == VERTEX_SHADER_TYPE);
            break;
        }
    }
    
    std::vector<Type> appropriateModules;
    for(const auto& file : files) {
        if(file.type == type) continue;
        
        if(isVertexShader) {
            if(file.shaderType == VERTEX_SHADER_TYPE) {
                appropriateModules.push_back(file.type);
            }
        } else {
            if(file.shaderType == FRAG_SHADER_TYPE) {
                appropriateModules.push_back(file.type);
            }
        }
    }
    
    size_t mainPos = mainShader.find("void main()");
    if(mainPos == std::string::npos) {
        return mainShader + concatModules(appropriateModules);
    }

    std::string before = mainShader.substr(0, mainPos);
    std::string after = mainShader.substr(mainPos);
    std::string result = before + concatModules(appropriateModules) + after;
    return result;
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

/**
 * Load
 */
std::string ShaderLoader::loadShader(const std::string& fileName) {
    std::string content = loadFile(fileName);
    content = processIncudes(content, fileName);
    printf("\n=== SHADERS: %s ===\n%s\n", fileName.c_str(), content.c_str());
    return content;
}

std::string ShaderLoader::loadFile(const std::string& fileName) {
    Type fileType;
    printf("Loading file: %s\n", fileName.c_str());

    for(const auto& file : files) {
        if(file.fileName == fileName) {
            fileType = file.type;
            break;
        }
    }
    auto it = loadedData.find(fileType);
    if(it != loadedData.end()) {
        return it->second;
    }

    return "";
}

void ShaderLoader::load() {
    pendingLoads = files.size();
    for(const auto& file : files) {
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = onSuccess;
        attr.onerror = onError;

        std::string url = file.fileName;
        addUrl(url, file.type);
        emscripten_fetch(&attr, url.c_str());
    }
}