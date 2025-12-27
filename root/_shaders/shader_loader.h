#pragma once
#include <functional>
#include <string>
#include <sstream>
#include <emscripten/fetch.h>

enum Type {
    VERTEX,
    FRAG,
    COLOR,
    TEXTURE,
    AMBIENT_LIGHT,
    POINT_LIGHT,
    SKYBOX,
    FRESNEL,
    NOISE
};

enum ShaderType {
    VERTEX_SHADER_TYPE,
    FRAG_SHADER_TYPE
};

struct Request {
    std::string url;
    Type type;
};

struct File {
    std::string fileName;
    Type type;
    ShaderType shaderType;
};

class ShaderController;
class ShaderLoader {
    private:
        static std::vector<File> files;
        static std::unordered_map<Type, std::string> loadedData;
        static std::vector<Request> request;
        static std::function<void()> dataCallback;
        static int pendingLoads;

        static void onSuccess(emscripten_fetch_t * fetch);
        static void onError(emscripten_fetch_t *fetch);

        static std::string processIncudes(const std::string& content, const std::string& parentFile);
        static std::string loadFile(const std::string& fileName);
        static std::string getParentDir(const std::string& path);
        static std::string stripVersionDir(const std::string& content);
    public:
        ShaderLoader();
        ~ShaderLoader();
        ShaderController* shaderController;

        static void onDataLoaded();
        static void setCallback(std::function<void()> callback);
        
        static void addUrl(const std::string& url, Type type);
        static std::string loadShader(const std::string& fileName);

        static void load();
        static std::string getShader(Type type);
        static std::string concatModules(const std::vector<Type>& funcTypes);
};