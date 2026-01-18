#define STB_IMAGE_IMPLEMENTATION
#include "texture_loader.h"
#include "base64_decoder.h"
#include "stb_image.h"
#include <iostream>
#include <vector>

TextureLoader::TextureLoader() {};
TextureLoader::~TextureLoader() {
    for(auto& p : textures) glDeleteTextures(1, &p.second);
    textures.clear();
};

/**
 * Load Texture
 */
GLuint TextureLoader::loadTexture(
    const std::string& name,
    const std::string& data,
    int width,
    int height
) {
    try {
        std::vector<unsigned char> imgData = Base64Decoder::decode(data);
        if(imgData.empty()) {
            std::cerr << "ERROR: Decoded image data is empty!" << std::endl;
            return 0;
        }

        int channels;
        int imgWidth;
        int imgHeight;
        unsigned char* pixels = stbi_load_from_memory(
            imgData.data(),
            imgData.size(),
            &imgWidth,
            &imgHeight,
            &channels,
            0
        );
        if(!pixels) {
            std::cerr << "stbi_load_from_memory failed: " << stbi_failure_reason() << std::endl;
            return 0;
        }

        GLuint texId = loadTextureFromMemory(pixels, imgWidth, imgHeight, channels);
        stbi_image_free(pixels);
        
        if(texId != 0) {
            textures[name] = texId;
        } else {
            std::cerr << "Failed to create texture" << std::endl;
        }
        
        return texId;
    } catch(const std::exception& err) {
        std::cerr << "error in loadTexture: " << err.what() << std::endl;
        return 0;
    }
}

/**
 * Load Texture From Memory
 */
GLuint TextureLoader::loadTextureFromMemory(
    const unsigned char* data,
    int width,
    int height,
    int channels
) {
    if(width <= 0 || height <= 0 || channels <= 0 || channels > 4) {
        std::cerr << "Invalid texture dimensions or channels: " 
                  << width << "x" << height << " channels: " << channels << std::endl;
        return 0;
    }
    
    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    GLenum format;
    switch(channels) {
        case 1:
            format = GL_LUMINANCE;
            break;
        case 2:
            format = GL_LUMINANCE_ALPHA;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            std::cerr << "Unsupported number of channels: " << channels << std::endl;
            glDeleteTextures(1, &texId);
            return 0;
    }
    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        format,
        width, 
        height, 
        0, 
        format, 
        GL_UNSIGNED_BYTE, 
        data
    );
    
    GLenum error = glGetError();
    if(error != GL_NO_ERROR) {
        std::cerr << "OpenGL error after glTexImage2D: 0x" << std::hex << error << std::dec << std::endl;
        
        if(error == GL_INVALID_OPERATION) {
            std::cout << "Retrying with simple format..." << std::endl;
            glTexImage2D(
                GL_TEXTURE_2D, 
                0, 
                format,
                width, 
                height, 
                0, 
                format, 
                GL_UNSIGNED_BYTE, 
                data
            );
            
            error = glGetError();
            if(error != GL_NO_ERROR) {
                std::cerr << "Still failing after retry: 0x" << std::hex << error << std::dec << std::endl;
                glDeleteTextures(1, &texId);
                return 0;
            }
        } else {
            glDeleteTextures(1, &texId);
            return 0;
        }
    }
    
    bool isPowerOfTwo = (width & (width - 1)) == 0 && (height & (height - 1)) == 0;
    
    if(isPowerOfTwo) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    
    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    
    if(width > maxTextureSize || height > maxTextureSize) {
        std::cerr << "ERROR: Texture size " << width << "x" << height 
                  << " exceeds WebGL maximum " << maxTextureSize << std::endl;
        glDeleteTextures(1, &texId);
        return 0;
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    
    std::cout << "  Texture created successfully. ID: " << texId 
              << " Size: " << width << "x" << height 
              << " (NPOT: " << (!isPowerOfTwo ? "yes" : "no") << ")" << std::endl;
    
    return texId;
}

/**
 * Load texture from File
 */
GLuint TextureLoader::loadTextureFromFile(const std::string& name, const std::string& filePath) {
    try {
        int width;
        int height;
        int channels;
        unsigned char* pixels = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
        if(!pixels) {
            std::cerr << "stbi_load failed for file " << filePath << ": " << stbi_failure_reason() << std::endl;
            return 0;
        }

        GLuint texId = loadTextureFromMemory(pixels, width, height, channels);
        stbi_image_free(pixels);
        if(texId != 0) {
            textures[name] = texId;
            std::cout << "Texture loaded from file: " << name << " (" << width << "x" << height << ")" << std::endl;
        } else {
            std::cerr << "Failed to create texture from file: " << name << std::endl;
        }

        return texId;
    } catch(const std::exception& err) {
        std::cerr << "error in loadTextureFromFile: " << err.what() << std::endl;
        return 0;
    }
}

/**
 * Preload Texture
 */
void TextureLoader::preloadTexture() {
    loadTextureFromFile("clouds", "/_resource/texture/clouds.png");
}

/**
 * Unload Texture
 */
void TextureLoader::unloadTexture(GLuint texId) {
    glDeleteTextures(1, &texId);
}

/**
 * Texture Exists
 */
bool TextureLoader::texExists(const std::string& texName) const {
    return textures.find(texName) != textures.end();
}

/**
 * Get Texture
 */
GLuint TextureLoader::getTex(const std::string& texName) const {
    auto it = textures.find(texName);
    return it != textures.end() ? it->second : 0;
}

/**
 * Add Texture
 */
void TextureLoader::addTex(const std::string& name, GLuint texId) {
    textures[name] = texId;
}