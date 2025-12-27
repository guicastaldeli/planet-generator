#pragma once
#include <functional>
#include <string>
#include <sstream>
#include <emscripten/fetch.h>
#include "shader_path.h"

struct Request {
    std::string url;
    ShaderPath::Type type;
};

class ShaderController;
class ShaderLoader {
    private:
        static std::unordered_map<ShaderPath::Type, std::string> loadedData;
        static std::vector<Request> request;
        static std::function<void()> dataCallback;
        static int pendingLoads;

        static void onSuccess(emscripten_fetch_t * fetch);
        static void onError(emscripten_fetch_t *fetch);

        static std::string processIncudes(const std::string& content, const std::string& parentFile);
        static std::string removeDuplicateUniforms(const std::string& content);
        static std::string loadFile(const std::string& fileName);
        static std::string loadFileByPath(const std::string& fileName);
        static std::string getParentDir(const std::string& path);
        static std::string stripVersionDir(const std::string& content);
        static std::string resolveIncludePath(const std::string& parentFile, const std::string& includePath);
    public:
        ShaderLoader();
        ~ShaderLoader();
        static ShaderLoader* instance;
        ShaderController* shaderController;
        ShaderPath* shaderPath;

        static void onDataLoaded();
        static void setCallback(std::function<void()> callback);
        
        static void addUrl(const std::string& url, ShaderPath::Type type);
        static std::string loadShader(const std::string& fileName);

        static void load();
        static std::string getShader(ShaderPath::Type type);
        static ShaderLoader* getInstance();
        static ShaderPath* getShaderPath();
};