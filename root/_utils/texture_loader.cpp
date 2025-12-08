#include "texture_loader.h"
#include "base64_decoder.h"
#include <iostream>
#include <vector>

TextureLoader::TextureLoader() {};
TextureLoader::~TextureLoader() {
    for(auto& p : textures) {
        glDeleteTextures(1, &p.second);
    }
    textures.clear();
};

GLuint TextureLoader::loadTexture(
    const std::string& name,
    const std::string& data,
    int width,
    int height
) {
    try {
        std::vector<unsigned char> imgData = Base64Decoder::decode(data);
        if(imgData.empty()) {
            std::cerr << "Failed to decode base64 image data!" << name << std::endl;
            return 0;
        }
    
        GLuint texId = loadTextureFromMemory(imgData.data(), width, height);
        if(texId != 0) {
            textures[name] = texId;
            std::cout << "Texture loaded successfully: " 
                << name << " (" 
                << width << "x" << height << ")" 
                << std::endl;
        }
    
        return texId;
    } catch(const std::exception& err) {
        std::cerr << "Error loading texture from base64: " << err.what() << std::endl;
        return 0;
    }
}

/*
 * Load Texture Memory
 */
GLuint TextureLoader::loadTextureFromMemory(
    const unsigned char* data,
    int width,
    int height
) {
    GLuint texId;

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texId;
}

/*
 * Unload Texture
 */
void TextureLoader::unloadTexture(GLuint texId) {
    glDeleteTextures(1, &texId);
}

/*
 * Texture Exists
 */
bool TextureLoader::texExists(const std::string& texName) const {
    return textures.find(texName) != textures.end();
}

/*
 * Get Texture
 */
GLuint TextureLoader::getTex(const std::string& texName) const {
    auto it = textures.find(texName);
    return it != textures.end() ? it->second : 0;
}

/*
 * Add Texture
 */
void TextureLoader::addTex(const std::string& name, GLuint texId) {
    textures[name] = texId;
}





