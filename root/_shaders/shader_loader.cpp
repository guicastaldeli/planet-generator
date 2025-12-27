#include "shader_loader.h"
#include <fstream>
#include <string>
#include <cstring>
#include <unordered_set>
#include <emscripten/fetch.h>
#include <emscripten/emscripten.h>
#include "shader_controller.h"

ShaderLoader* ShaderLoader::instance = nullptr;
ShaderLoader::ShaderLoader() {
    shaderController = new ShaderController();
    if(!shaderPath) shaderPath = new ShaderPath();
    instance = this;
}
ShaderLoader::~ShaderLoader() {}

std::unordered_map<ShaderPath::Type, std::string> ShaderLoader::loadedData;
std::function<void()> ShaderLoader::dataCallback = nullptr;
std::vector<Request> ShaderLoader::request;
int ShaderLoader::pendingLoads = 0;

void ShaderLoader::onDataLoaded() {
    if(dataCallback) dataCallback();
}

void ShaderLoader::setCallback(std::function<void()> callback) {
    dataCallback = callback;
}

void ShaderLoader::addUrl(const std::string& url, ShaderPath::Type type) {
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

std::string ShaderLoader::resolveIncludePath(const std::string& parentFile, const std::string& includePath) {
    size_t lastSlash = parentFile.find_last_of('/');
    std::string parentDir;
    if(lastSlash != std::string::npos) {
        parentDir = parentFile.substr(0, lastSlash + 1);
    } else {
        parentDir = "./";
    }
    
    if(includePath[0] == '/') {
        return includePath;
    }
    
    std::string result = parentDir;
    std::string path = includePath;
    
    while(path.find("../") == 0) {
        path = path.substr(3);
        
        size_t slashPos = result.find_last_of('/', result.length() - 2);
        if(slashPos != std::string::npos) {
            result = result.substr(0, slashPos + 1);
        } else {
            break;
        }
    }
    
    result += path;
    return result;
}


/**
 * Process Includes
 */
std::string ShaderLoader::processIncludes(const std::string& content, const std::string& parentFile) {
    std::string result;
    std::stringstream ss(content);
    std::string line;
    
    while(std::getline(ss, line)) {
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
        
        if(trimmed.find("#include ") == 0) {
            size_t fQuote = trimmed.find('"');
            size_t sQuote = trimmed.find('"', fQuote + 1);
            if(fQuote != std::string::npos && sQuote != std::string::npos) {
                std::string includeFile = trimmed.substr(fQuote + 1, sQuote - fQuote - 1);
                std::string resolvedPath = resolveIncludePath(parentFile, includeFile);
        
                printf("Resolved include: '%s' -> '%s'\n", includeFile.c_str(), resolvedPath.c_str());
                
                std::string includeContent = loadFileByPath(resolvedPath);
                if(!includeContent.empty()) {
                    includeContent = processIncludes(includeContent, resolvedPath);
                    includeContent = stripVersionDir(includeContent);
                    includeContent = removeDuplicateUniforms(includeContent);
                    result += includeContent + "\n";
                } else {
                    printf("WARNING: Could not load include file: %s (resolved as: %s)\n", 
                           includeFile.c_str(), resolvedPath.c_str());
                }
            }
        } else {
            result += line + "\n";
        }
    }
    return result;
}

std::string ShaderLoader::removeDuplicateUniforms(const std::string& content) {
    std::stringstream ss(content);
    std::string result;
    std::string line;
    std::unordered_set<std::string> declaredUniforms;
    
    while(std::getline(ss, line)) {
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        
        if(trimmed.find("uniform ") == 0) {
            size_t typeStart = trimmed.find(' ', 7);
            if(typeStart != std::string::npos) {
                size_t nameStart = trimmed.find_first_not_of(" \t", typeStart);
                if(nameStart != std::string::npos) {
                    size_t nameEnd = trimmed.find_first_of(" ;[", nameStart);
                    if(nameEnd != std::string::npos) {
                        std::string uniformName = trimmed.substr(nameStart, nameEnd - nameStart);
                        
                        if(declaredUniforms.find(uniformName) == declaredUniforms.end()) {
                            declaredUniforms.insert(uniformName);
                            result += line + "\n";
                        }
                        continue;
                    }
                }
            }
        }
        result += line + "\n";
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
std::string ShaderLoader::getShader(ShaderPath::Type type) {
    if(loadedData.find(type) == loadedData.end()) {
        printf("Main shader type %d not found\n", type);
        return "";
    }

    std::string mainShader = loadedData[type];
    std::string filePath;
    
    for(const auto& file : ShaderPath::files) {
        if(file.type == type) {
            filePath = file.fileName;
            break;
        }
    }
    
    std::string processed = processIncludes(mainShader, filePath);
    
    printf("\n\n■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
    printf("SHADER: %s (Type: %d)\n", filePath.c_str(), type);
    printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
    printf("%s\n", processed.c_str());
    printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n");
    
    return processed;
}

/**
 * Load
 */
std::string ShaderLoader::loadShader(const std::string& fileName) {
    std::string content = loadFile(fileName);
    content = processIncludes(content, fileName);
    printf("\n=== SHADERS: %s ===\n%s\n", fileName.c_str(), content.c_str());
    return content;
}

std::string ShaderLoader::loadFile(const std::string& fileName) {
    ShaderPath::Type fileShaderPath;
    printf("Loading file: %s\n", fileName.c_str());

    for(const auto& file : ShaderPath::files) {
        if(file.fileName == fileName) {
            fileShaderPath = file.type;
            break;
        }
    }
    auto it = loadedData.find(fileShaderPath);
    if(it != loadedData.end()) {
        return it->second;
    }

    return "";
}

std::string ShaderLoader::loadFileByPath(const std::string& fileName) {
    for(const auto& file : ShaderPath::files) {
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
    pendingLoads = ShaderPath::files.size();
    for(const auto& file : ShaderPath::files) {
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

ShaderLoader* ShaderLoader::getInstance() {
    return instance;
}

/**
 * Get ShaderPath
 */
ShaderPath* ShaderLoader::getShaderPath() {
    if(instance) return instance->shaderPath;
    return nullptr;
}