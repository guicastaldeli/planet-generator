#include "buffers.h"
#include "../_shaders/shader_controller.h"
#include "../camera.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Buffers::Buffers(
    Camera* camera, 
    ShaderController* shaderController, 
    BufferController* bufferController
) :
    camera(camera),
    shaderController(shaderController),
    bufferController(bufferController),
    isPreviewMode(false)
{}
Buffers::~Buffers() {
    for(auto& [type, v] : vaos) {
        glDeleteVertexArrays(1, &v);
        glDeleteBuffers(1, &vbos[type]);
        glDeleteBuffers(1, &ebos[type]);
    } 
}

/**
 * Set Buffers
 */
void Buffers::set(BufferData::Type type) {
    if(vaos.find(type) != vaos.end()) return;

    BufferData::MeshData meshData = BufferData::GetMeshData(type);
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER, 
        meshData.vertices.size() * sizeof(float),
        meshData.vertices.data(), 
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, 
        meshData.indices.size() * sizeof(GLuint),
        meshData.indices.data(), 
        GL_STATIC_DRAW
    );

    GLsizei stride = 8 * sizeof(float);
    GLuint posAttr = glGetAttribLocation(shaderController->shaderProgram, "aPos");
    if(posAttr != -1) {
        glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(posAttr);
    }

    GLuint normalAttr = glGetAttribLocation(shaderController->shaderProgram, "aNormal");
    if(normalAttr != -1) {
        glVertexAttribPointer(normalAttr, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(normalAttr);
    }

    GLuint texCoordAttr = glGetAttribLocation(shaderController->shaderProgram, "aTexCoord");
    if(texCoordAttr != -1) {
        glVertexAttribPointer(texCoordAttr, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(texCoordAttr);
    }

    glBindVertexArray(0);

    vaos[type] = vao;
    vbos[type] = vbo;
    ebos[type] = ebo;
    indexCounts[type] = meshData.indices.size();
}

/**
 * Create Buffer
 */
void Buffers::createBufferForPlanet(const PlanetBuffer& planetBuffer) {
    set(planetBuffer.data.shape);
}

/**
 * Render
 */
void Buffers::render() {
    glUseProgram(shaderController->shaderProgram);
    
    /* Normal Mesh */
    if(!isPreviewMode) {
        for(auto& planetBuffer : planetBuffers) {
            auto it = vaos.find(planetBuffer.data.shape);
            if(it == vaos.end()) continue;
    
            glBindVertexArray(it->second);
    
            static float previewRotation = 0.0f;
            previewRotation += 0.5f;
    
            float orbitRadius = planetBuffer.data.distanceFromCenter;
            float orbitAngle = planetBuffer.data.orbitAngle.y;
            planetBuffer.worldPos = glm::vec3(
                orbitRadius * cos(glm::radians(orbitAngle)),
                0.0f,
                orbitRadius * sin(glm::radians(orbitAngle))
            );

            GLuint shaderTypeLoc = glGetUniformLocation(shaderController->shaderProgram, "shaderType");
            if(shaderTypeLoc != -1) glUniform1f(shaderTypeLoc, 0.0f);

            GLuint emissiveStrengthLoc = glGetUniformLocation(shaderController->shaderProgram, "uEmissiveStrength");
            if(emissiveStrengthLoc != -1) {
                float emissiveStrength = planetBuffer.data.hasSunLight ? 1.5f : 0.0;
                glUniform1f(emissiveStrengthLoc, emissiveStrength);
            }
    
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, planetBuffer.worldPos);
            model = glm::rotate(model, planetBuffer.data.currentRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(planetBuffer.data.size));
            unsigned int modelLoc = glGetUniformLocation(shaderController->shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            
            GLuint planetColorLoc = glGetUniformLocation(shaderController->shaderProgram, "pColor");
            if(planetColorLoc != -1) {
                glm::vec3 color = planetBuffer.data.colorRgb;
                glUniform3f(planetColorLoc, color.r, color.g, color.b);
            }

            GLuint useTexLoc = glGetUniformLocation(shaderController->shaderProgram, "uUseTex");
            bool hasTex = 
                !planetBuffer.data.texture.empty() &&
                bufferController->getTextureLoader()->texExists(planetBuffer.data.texture);
            if(useTexLoc != -1) {
                glUniform1i(useTexLoc, hasTex ? 1 : 0);
            }
            if(hasTex) {
                GLuint texLoc = glGetUniformLocation(shaderController->shaderProgram, "uTex");
                GLuint texId = bufferController->getTextureLoader()->getTex(planetBuffer.data.texture);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texId);
                glUniform1i(texLoc, 0);
            }

            GLuint hoverLoc = glGetUniformLocation(shaderController->shaderProgram, "isHovered"); 
            int isThisPlanetHovered = (
                bufferController->raycaster->selectedPlanetIndex == &planetBuffer - &planetBuffers[0]
            ) ? 1 : 0;
            if(hoverLoc != -1) {
                glUniform1f(hoverLoc, (float)isThisPlanetHovered);
            }

            GLuint effectTypeLoc = glGetUniformLocation(shaderController->shaderProgram, "uEffectType");
            if(effectTypeLoc != -1) {
                glUniform1f(effectTypeLoc, static_cast<float>(planetBuffer.data.effectType));
            }

            GLuint planetSizeLoc = glGetUniformLocation(shaderController->shaderProgram, "uPlanetSize");
            if(planetSizeLoc != -1) {
                glUniform1f(planetSizeLoc, planetBuffer.data.size);
            }
    
            glDrawElements(
                GL_TRIANGLES,
                indexCounts[planetBuffer.data.shape],
                GL_UNSIGNED_INT,
                0
            );

            if(planetBuffer.data.effectType == 1) renderAtmosphere(planetBuffer);
            if(planetBuffer.data.effectType == 3) renderClouds(planetBuffer);
            if(planetBuffer.data.effectType == 4) renderRings(planetBuffer);
        }
    }
    /* Preview Mesh */
    if(!previewPlanet.data.name.empty()) {
        auto it = vaos.find(previewPlanet.data.shape);
        if(it != vaos.end()) {
            glBindVertexArray(it->second);

            static float previewRotation = 0.0f;
            previewRotation += 0.5f;

            float screenX = -0.5f;
            float screenY = 0.0f;
            float screenZ = 0.0f;
            glUseProgram(shaderController->shaderProgram);

            glm::mat4 view = glm::lookAt(
                glm::vec3(0.0f, 0.0f, 5.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            unsigned int viewLoc = glGetUniformLocation(shaderController->shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(screenX, screenY, screenZ));
            
            if(previewPlanet.data.rotationDir == RotationAxis::X) {
                model = glm::rotate(model, glm::radians(previewRotation * previewPlanet.data.rotationSpeedItself), glm::vec3(1.0f, 0.0f, 0.0f));
            } else if(previewPlanet.data.rotationDir == RotationAxis::Y) {
                model = glm::rotate(model, glm::radians(previewRotation * previewPlanet.data.rotationSpeedItself), glm::vec3(0.0f, 1.0f, 0.0f));
            } else if(previewPlanet.data.rotationDir == RotationAxis::Z) {
                model = glm::rotate(model, glm::radians(previewRotation * previewPlanet.data.rotationSpeedItself), glm::vec3(0.0f, 0.0f, 1.0f));
            }
            model = glm::scale(model, glm::vec3(previewPlanet.data.size));
            unsigned int modelLoc = glGetUniformLocation(shaderController->shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            GLuint emissiveStrengthLoc = glGetUniformLocation(shaderController->shaderProgram, "uEmissiveStrength");
            if(emissiveStrengthLoc != -1) {
                float emissiveStrength = previewPlanet.data.hasSunLight ? 1.0f : 0.0;
                glUniform1f(emissiveStrengthLoc, emissiveStrength);
            }

            GLuint planetColorLoc = glGetUniformLocation(shaderController->shaderProgram, "pColor");
            if(planetColorLoc != -1) {
                glm::vec3 color = previewPlanet.data.colorRgb;
                glUniform3f(planetColorLoc, color.r, color.g, color.b);
            }

            GLuint useTexLoc = glGetUniformLocation(shaderController->shaderProgram, "uUseTex");
            bool hasTex = 
                !previewPlanet.data.texture.empty() &&
                bufferController->getTextureLoader()->texExists(previewPlanet.data.texture);
            if(useTexLoc != -1) {
                glUniform1i(useTexLoc, hasTex ? 1 : 0);
            }
            if(hasTex) {
                GLuint texLoc = glGetUniformLocation(shaderController->shaderProgram, "uTex");
                GLuint texId = bufferController->getTextureLoader()->getTex(previewPlanet.data.texture);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texId);
                glUniform1i(texLoc, 0);
            }

            GLuint hoverLoc = glGetUniformLocation(shaderController->shaderProgram, "isHovered"); 
            if(hoverLoc != -1) glUniform1f(hoverLoc, 0.0f);

            GLuint effectTypeLoc = glGetUniformLocation(shaderController->shaderProgram, "uEffectType");
            if(effectTypeLoc != -1) {
                glUniform1f(effectTypeLoc, static_cast<float>(previewPlanet.data.effectType));
            }

            GLuint planetSizeLoc = glGetUniformLocation(shaderController->shaderProgram, "uPlanetSize");
            if(planetSizeLoc != -1) {
                glUniform1f(planetSizeLoc, previewPlanet.data.size);
            }

            glDrawElements(
                GL_TRIANGLES,
                indexCounts[previewPlanet.data.shape],
                GL_UNSIGNED_INT,
                0
            );

            if(previewPlanet.data.effectType == 1) renderAtmosphere(previewPlanet);
            if(previewPlanet.data.effectType == 3) renderClouds(previewPlanet);
            if(previewPlanet.data.effectType == 4) renderRings(previewPlanet);
        }
    }

    glBindVertexArray(0);
}

/**
 * Render Rings
 */
void Buffers::renderRings(const PlanetBuffer& planetBuffer) {
    GLboolean depthMask;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
    GLboolean blendEnabled = glIsEnabled(GL_BLEND);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glm::mat4 model = glm::mat4(1.0f);
    
    if(planetBuffer.isPreview) {
        float screenX = -0.5f;
        float screenY = 0.0f;
        model = glm::translate(model, glm::vec3(screenX, screenY, 0.0f));
        
        static float previewRotation = 0.0f;
        previewRotation += 0.5f;
        
        if(planetBuffer.data.rotationDir == RotationAxis::X) {
            model = glm::rotate(model, glm::radians(previewRotation * planetBuffer.data.rotationSpeedItself), glm::vec3(1.0f, 0.0f, 0.0f));
        } else if(planetBuffer.data.rotationDir == RotationAxis::Y) {
            model = glm::rotate(model, glm::radians(previewRotation * planetBuffer.data.rotationSpeedItself), glm::vec3(0.0f, 1.0f, 0.0f));
        } else if(planetBuffer.data.rotationDir == RotationAxis::Z) {
            model = glm::rotate(model, glm::radians(previewRotation * planetBuffer.data.rotationSpeedItself), glm::vec3(0.0f, 0.0f, 1.0f));
        }
    } else {
        float orbitRadius = planetBuffer.data.distanceFromCenter;
        float orbitAngle = planetBuffer.data.orbitAngle.y;
        glm::vec3 worldPos = glm::vec3(
            orbitRadius * cos(glm::radians(orbitAngle)),
            0.0f,
            orbitRadius * sin(glm::radians(orbitAngle))
        );
        
        model = glm::translate(model, worldPos);
        model = glm::rotate(model, planetBuffer.data.currentRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    float atmosphereScale = planetBuffer.data.size * 1.05f;
    model = glm::scale(model, glm::vec3(atmosphereScale));

    unsigned int modelLoc = glGetUniformLocation(shaderController->shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    GLuint planetColorLoc = glGetUniformLocation(shaderController->shaderProgram, "pColor");
    if(planetColorLoc != -1) {
        glm::vec3 atmosphereColor = glm::vec3(0.2f, 0.4f, 1.0f);
        glUniform3f(planetColorLoc, atmosphereColor.r, atmosphereColor.g, atmosphereColor.b);
    }

    GLuint emissiveStrengthLoc = glGetUniformLocation(shaderController->shaderProgram, "uEmissiveStrength");
    if(emissiveStrengthLoc != -1) {
        glUniform1f(emissiveStrengthLoc, 0.7f);
    }
    
    GLuint isAtmosphereLoc = glGetUniformLocation(shaderController->shaderProgram, "uIsAtmosphere");
    if(isAtmosphereLoc != -1) {
        glUniform1f(isAtmosphereLoc, 1.0f);
    }
    
    GLuint useTexLoc = glGetUniformLocation(shaderController->shaderProgram, "uUseTex");
    if(useTexLoc != -1) {
        glUniform1i(useTexLoc, 0);
    }
    
    glDrawElements(
        GL_TRIANGLES,
        indexCounts[planetBuffer.data.shape],
        GL_UNSIGNED_INT,
        0
    );

    if(isAtmosphereLoc != -1) {
        glUniform1f(isAtmosphereLoc, 0.0f);
    }
    if(emissiveStrengthLoc != -1) {
        float originalEmissive = planetBuffer.data.hasSunLight ? 1.5f : 0.0;
        glUniform1f(emissiveStrengthLoc, originalEmissive);
    }
    glDepthMask(depthMask);
    if(!blendEnabled) {
        glDisable(GL_BLEND);
    }
}

/**
 * Render Clouds
 */
void Buffers::renderClouds(const PlanetBuffer& planetBuffer) {
    GLboolean depthMask;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
    GLboolean blendEnabled = glIsEnabled(GL_BLEND);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glm::mat4 model = glm::mat4(1.0f);
    
    if(planetBuffer.isPreview) {
        float screenX = -0.5f;
        float screenY = 0.0f;
        model = glm::translate(model, glm::vec3(screenX, screenY, 0.0f));
        
        static float previewRotation = 0.0f;
        previewRotation += 0.5f;
        
        if(planetBuffer.data.rotationDir == RotationAxis::X) {
            model = glm::rotate(model, glm::radians(previewRotation * planetBuffer.data.rotationSpeedItself), glm::vec3(1.0f, 0.0f, 0.0f));
        } else if(planetBuffer.data.rotationDir == RotationAxis::Y) {
            model = glm::rotate(model, glm::radians(previewRotation * planetBuffer.data.rotationSpeedItself), glm::vec3(0.0f, 1.0f, 0.0f));
        } else if(planetBuffer.data.rotationDir == RotationAxis::Z) {
            model = glm::rotate(model, glm::radians(previewRotation * planetBuffer.data.rotationSpeedItself), glm::vec3(0.0f, 0.0f, 1.0f));
        }
    } else {
        float orbitRadius = planetBuffer.data.distanceFromCenter;
        float orbitAngle = planetBuffer.data.orbitAngle.y;
        glm::vec3 worldPos = glm::vec3(
            orbitRadius * cos(glm::radians(orbitAngle)),
            0.0f,
            orbitRadius * sin(glm::radians(orbitAngle))
        );
        
        model = glm::translate(model, worldPos);
        model = glm::rotate(model, planetBuffer.data.currentRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    float atmosphereScale = planetBuffer.data.size * 1.05f;
    model = glm::scale(model, glm::vec3(atmosphereScale));

    unsigned int modelLoc = glGetUniformLocation(shaderController->shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    GLuint planetColorLoc = glGetUniformLocation(shaderController->shaderProgram, "pColor");
    if(planetColorLoc != -1) {
        glm::vec3 atmosphereColor = glm::vec3(0.2f, 0.4f, 1.0f);
        glUniform3f(planetColorLoc, atmosphereColor.r, atmosphereColor.g, atmosphereColor.b);
    }

    GLuint emissiveStrengthLoc = glGetUniformLocation(shaderController->shaderProgram, "uEmissiveStrength");
    if(emissiveStrengthLoc != -1) {
        glUniform1f(emissiveStrengthLoc, 0.7f);
    }
    
    GLuint isAtmosphereLoc = glGetUniformLocation(shaderController->shaderProgram, "uIsAtmosphere");
    if(isAtmosphereLoc != -1) {
        glUniform1f(isAtmosphereLoc, 1.0f);
    }
    
    GLuint useTexLoc = glGetUniformLocation(shaderController->shaderProgram, "uUseTex");
    if(useTexLoc != -1) {
        glUniform1i(useTexLoc, 0);
    }
    
    glDrawElements(
        GL_TRIANGLES,
        indexCounts[planetBuffer.data.shape],
        GL_UNSIGNED_INT,
        0
    );

    if(isAtmosphereLoc != -1) {
        glUniform1f(isAtmosphereLoc, 0.0f);
    }
    if(emissiveStrengthLoc != -1) {
        float originalEmissive = planetBuffer.data.hasSunLight ? 1.5f : 0.0;
        glUniform1f(emissiveStrengthLoc, originalEmissive);
    }
    glDepthMask(depthMask);
    if(!blendEnabled) {
        glDisable(GL_BLEND);
    }
}

/**
 * Render Atmosphere
 */
void Buffers::renderAtmosphere(const PlanetBuffer& planetBuffer) {
    GLboolean depthMask;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
    GLboolean blendEnabled = glIsEnabled(GL_BLEND);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glm::mat4 model = glm::mat4(1.0f);
    
    if(planetBuffer.isPreview) {
        float screenX = -0.5f;
        float screenY = 0.0f;
        model = glm::translate(model, glm::vec3(screenX, screenY, 0.0f));
        
        static float previewRotation = 0.0f;
        previewRotation += 0.5f;
        
        if(planetBuffer.data.rotationDir == RotationAxis::X) {
            model = glm::rotate(model, glm::radians(previewRotation * planetBuffer.data.rotationSpeedItself), glm::vec3(1.0f, 0.0f, 0.0f));
        } else if(planetBuffer.data.rotationDir == RotationAxis::Y) {
            model = glm::rotate(model, glm::radians(previewRotation * planetBuffer.data.rotationSpeedItself), glm::vec3(0.0f, 1.0f, 0.0f));
        } else if(planetBuffer.data.rotationDir == RotationAxis::Z) {
            model = glm::rotate(model, glm::radians(previewRotation * planetBuffer.data.rotationSpeedItself), glm::vec3(0.0f, 0.0f, 1.0f));
        }
    } else {
        float orbitRadius = planetBuffer.data.distanceFromCenter;
        float orbitAngle = planetBuffer.data.orbitAngle.y;
        glm::vec3 worldPos = glm::vec3(
            orbitRadius * cos(glm::radians(orbitAngle)),
            0.0f,
            orbitRadius * sin(glm::radians(orbitAngle))
        );
        
        model = glm::translate(model, worldPos);
        model = glm::rotate(model, planetBuffer.data.currentRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    float atmosphereScale = planetBuffer.data.size * 1.05f;
    model = glm::scale(model, glm::vec3(atmosphereScale));

    unsigned int modelLoc = glGetUniformLocation(shaderController->shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    GLuint planetColorLoc = glGetUniformLocation(shaderController->shaderProgram, "pColor");
    if(planetColorLoc != -1) {
        glm::vec3 atmosphereColor = glm::vec3(0.2f, 0.4f, 1.0f);
        glUniform3f(planetColorLoc, atmosphereColor.r, atmosphereColor.g, atmosphereColor.b);
    }

    GLuint emissiveStrengthLoc = glGetUniformLocation(shaderController->shaderProgram, "uEmissiveStrength");
    if(emissiveStrengthLoc != -1) {
        glUniform1f(emissiveStrengthLoc, 0.7f);
    }
    
    GLuint isAtmosphereLoc = glGetUniformLocation(shaderController->shaderProgram, "uIsAtmosphere");
    if(isAtmosphereLoc != -1) {
        glUniform1f(isAtmosphereLoc, 1.0f);
    }
    
    GLuint useTexLoc = glGetUniformLocation(shaderController->shaderProgram, "uUseTex");
    if(useTexLoc != -1) {
        glUniform1i(useTexLoc, 0);
    }
    
    glDrawElements(
        GL_TRIANGLES,
        indexCounts[planetBuffer.data.shape],
        GL_UNSIGNED_INT,
        0
    );

    if(isAtmosphereLoc != -1) {
        glUniform1f(isAtmosphereLoc, 0.0f);
    }
    if(emissiveStrengthLoc != -1) {
        float originalEmissive = planetBuffer.data.hasSunLight ? 1.5f : 0.0;
        glUniform1f(emissiveStrengthLoc, originalEmissive);
    }
    glDepthMask(depthMask);
    if(!blendEnabled) {
        glDisable(GL_BLEND);
    }
}

/**
 * 
 * Preview Planet
 * 
 */
void Buffers::setupPreviewPlanet(const PlanetData& data) {
    previewPlanet.data = data;
    previewPlanet.isPreview = true;
    previewPlanet.data.id = -1;
    set(previewPlanet.data.shape);
}

void Buffers::updatePreviewPlanet(const PlanetData& data) {
    if(previewPlanet.data.name.empty()) {
        setupPreviewPlanet(data);
        return;
    }

    previewPlanet.data = data;
    previewPlanet.isPreview = true;
    previewPlanet.data.id = -1;
    set(previewPlanet.data.shape);
}

void Buffers::setPreviewMode(bool preview) {
    isPreviewMode = preview;
}

bool Buffers::isInPreviewMode() const {
    return isPreviewMode;
}

void Buffers::cleanupPreviewPlanet() {
    previewPlanet = PlanetBuffer();
}

void Buffers::clearBuffers() {
    planetBuffers.clear();
}

bool Buffers::hasPreviewPlanet() const {
    return !previewPlanet.data.name.empty();
}

const PlanetBuffer& Buffers::getPreviewBuffer() const {
    return previewPlanet;
} 

/**
 * Init
 */
void Buffers::init() {
    emscripten_log(EM_LOG_CONSOLE, "init buffers!");
}