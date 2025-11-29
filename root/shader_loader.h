#include <functional>
#include <string>
#include <emscripten/fetch.h>


class ShaderLoader {
    private:
        static std::string loadedData;
        static std::function<void(const std::string&)> dataCallback;

        static void onSuccess(emscripten_fetch_t * fetch);
        static void onError(emscripten_fetch_t *fetch);
        
    public:
        static void onDataLoaded(const std::string& data);
        static void setCallback(std::function<void(const std::string&)> callback);
        void load();
};