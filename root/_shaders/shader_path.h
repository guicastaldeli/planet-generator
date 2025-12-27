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
            FRESNEL,
            NOISE
        };

        struct File {
            std::string fileName;
            Type type;
        };

        static std::vector<File> files;
}