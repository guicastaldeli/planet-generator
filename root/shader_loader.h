#pragma once
#include <functional>
#include <string>
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

struct Request {
    std::string url;
    Type type;
};

struct File {
    std::string fileName;
    Type type;
};

class ShaderController;
class ShaderLoader {
    private:
        std::vector<File> files = {
            { "vertex.glsl", VERTEX },
            { "frag.glsl", FRAG },
            { "color.glsl", COLOR} ,
            { "texture.glsl", TEXTURE },
            { "ambient_light.glsl", AMBIENT_LIGHT },
            { "point_light.glsl", POINT_LIGHT },
            { "skybox.glsl", SKYBOX },
            { "fresnel.glsl", FRESNEL },
            { "noise.glsl", NOISE }
        };
        static std::unordered_map<Type, std::string> loadedData;
        static std::vector<Request> request;
        static std::function<void()> dataCallback;
        static int pendingLoads;

        static void onSuccess(emscripten_fetch_t * fetch);
        static void onError(emscripten_fetch_t *fetch);
        
    public:
        ShaderLoader();
        ~ShaderLoader();
        ShaderController* shaderController;

        static void onDataLoaded();
        static void setCallback(std::function<void()> callback);
        
        static void addUrl(const std::string& url, Type type);
        void load();
        static const std::string& getShader(Type type);
};