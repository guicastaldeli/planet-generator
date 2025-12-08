#pragma once
#include <string>
#include <unordered_map>
#include <GLES3/gl3.h>

class TextureLoader {
    private:
        std::unordered_map<std::string, GLuint> textures;

    public:
        TextureLoader();
        ~TextureLoader();

        GLuint loadTexture(
            const std::string& name,
            const std::string& data,
            int width,
            int height
        );
        GLuint loadTextureFromMemory(const unsigned char* data, int width, int height);
        void unloadTexture(GLuint texId);
        bool texExists(const std::string& texName) const;
        GLuint getTex(const std::string& texName) const;
        void addTex(const std::string& name, GLuint texId);
};