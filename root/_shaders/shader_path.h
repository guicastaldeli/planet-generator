#include <fstream>
#include <string>
#include <cstring>

class ShaderPath {
    public:
        enum Type {
            VERTEX,
            FRAG,
            COLOR,
            TEXTURE,
            AMBIENT_LIGHT,
            POINT_LIGHT,
            SKYBOX_VERT,
            SKYBOX_FRAG,
            STAR_COLOR,
            STAR_ATTR,
            FRESNEL,
            GLOW,
            NOISE,
            CLOUDS,
            RINGS
        };

        struct File {
            std::string fileName;
            Type type;
        };

        static std::vector<File> files;
};