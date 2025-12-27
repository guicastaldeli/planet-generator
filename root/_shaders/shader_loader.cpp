#include "shader_loader.h"
#include <fstream>
#include <string>
#include <cstring>
#include <emscripten/fetch.h>
#include <emscripten/emscripten.h>
#include "shader_controller.h"

std::string path = "/root/_shaders/";
std::vector<File> ShaderLoader::files = {
    { path + "main/vertex.glsl", VERTEX },
    { path + "main/frag.glsl", FRAG },
    { path + "main/color.glsl", COLOR },
    { path + "main/texture.glsl", TEXTURE },
    { path + "lightning/ambient_light.glsl", AMBIENT_LIGHT },
    { path + "lightning/point_light.glsl", POINT_LIGHT },
    { path + "skybox/skybox.glsl", SKYBOX },
    { path + "effect/fresnel.glsl", FRESNEL },
    { path + "effect/noise.glsl", NOISE }
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
            size_t fQuote = trimmed.find('"');
            size_t sQuote = trimmed.find('"', fQuote + 1);
            if(fQuote != std::string::npos && sQuote != std::string::npos) {
                std::string file = trimmed.substr(fQuote + 1, sQuote - fQuote - 1);
                std::string path;
                if(file.find('/') != std::string::npos) {
                    path = file;
                } else {
                    path = parentDir + file;
                }

                printf("Found include: '%s' -> '%s'\n", file.c_str(), path.c_str());
                std::string includeContent = loadFileByPath(path);
                if(!includeContent.empty()) {
                    includeContent = processIncudes(includeContent, path);
                    includeContent = stripVersionDir(includeContent);
                    result += includeContent + "\n";
                } else {
                    result += line + "\n";
                }
            }
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
    std::string filePath;
    for(const auto& file : files) {
        if(file.type == type) {
            filePath = file.fileName;
            break;
        }
    }

    return processIncudes(mainShader, filePath);
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

std::string ShaderLoader::loadFileByPath(const std::string& fileName) {
    for(const auto& file : files) {
        if(file.fileName == fileName) {
            auto it = loadedData.find(file.type);
            if(it != loadedData.end()) {
                return it->second;
            }
        }
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