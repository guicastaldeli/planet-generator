#include <functional>
#include <string>
#include <emscripten/fetch.h>

enum Type {
    VERTEX,
    FRAG
};

struct Request {
    std::string url;
    Type type;
};

class ShaderLoader {
    private:
        static std::unordered_map<Type, std::string> loadedData;
        static std::function<void()> dataCallback;
        static std::vector<Request> request;
        static int pendingLoads;

        static void onSuccess(emscripten_fetch_t * fetch);
        static void onError(emscripten_fetch_t *fetch);
        
    public:
        static void onDataLoaded();
        static void setCallback(std::function<void()> callback);
        static void addUrl(const std::string& url, Type type);
        void load();
        static const std::string& getShader(Type type);
};