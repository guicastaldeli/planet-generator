#include "buffers.h"
#include "../.controller/shader_controller.h"
#include "../camera.h"
#include <emscripten.h>
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

/*
** Set Buffers
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

    // Position attribute (3 floats)
    GLuint posAttr = glGetAttribLocation(shaderController->shaderProgram, "aPos");
    if(posAttr != -1) {
        glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 
                              5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(posAttr);
    }

    // Texture coordinate attribute (2 floats, starts after 3 position floats)
    GLuint texCoordAttr = glGetAttribLocation(shaderController->shaderProgram, "aTexCoord");
    if(texCoordAttr != -1) {
        glVertexAttribPointer(texCoordAttr, 2, GL_FLOAT, GL_FALSE, 
                              5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(texCoordAttr);
    }

    glBindVertexArray(0);

    vaos[type] = vao;
    vbos[type] = vbo;
    ebos[type] = ebo;
    indexCounts[type] = meshData.indices.size();
}

/*
** Create Buffer for Planet
*/
void Buffers::createBufferForPlanet(const PlanetBuffer& planetBuffer) {
    set(planetBuffer.data.shape);
}

/*
** Render
*/
void Buffers::render() {
    glUseProgram(shaderController->shaderProgram);
    
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
                if(texLoc != -1 && texId != 0) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, texId);
                    glUniform1i(texLoc, 0);
                }
            }

            GLuint hoverLoc = glGetUniformLocation(shaderController->shaderProgram, "isHovered"); 
            int isThisPlanetHovered = (
                bufferController->raycaster->selectedPlanetIndex == &planetBuffer - &planetBuffers[0]
            ) ? 1 : 0;
            if(hoverLoc != -1) {
                glUniform1f(hoverLoc, (float)isThisPlanetHovered);
            }
    
            glDrawElements(
                GL_TRIANGLES,
                indexCounts[planetBuffer.data.shape],
                GL_UNSIGNED_INT,
                0
            );
        }
    }
     if(!previewPlanet.data.name.empty()) {
        auto it = vaos.find(previewPlanet.data.shape);
        if(it != vaos.end()) {
            glBindVertexArray(it->second);

            static float previewRotation = 0.0f;
            previewRotation += 0.5f;

            float screenX = 0.7f;
            float screenY = 0.0f;
            int screenWidth = camera->main->width;
            int screenHeight = camera->main->height;
            glUseProgram(shaderController->shaderProgram);

            glm::mat4 view = glm::lookAt(
                glm::vec3(0.0f, 0.0f, 5.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            unsigned int viewLoc = glGetUniformLocation(shaderController->shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(screenX, screenY, 0.0f));
            
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
                if(texLoc != -1 && texId != 0) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, texId);
                    glUniform1i(texLoc, 0);
                }
            }

            GLuint hoverLoc = glGetUniformLocation(shaderController->shaderProgram, "isHovered"); 
            if(hoverLoc != -1) glUniform1f(hoverLoc, 0.0f);

            glDrawElements(
                GL_TRIANGLES,
                indexCounts[previewPlanet.data.shape],
                GL_UNSIGNED_INT,
                0
            );
            
            if(!isPreviewMode) {
                camera->set();
            }
        }
    }

    glBindVertexArray(0);
}

/*
**
*** Preview Planet
**
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

void Buffers::cleanupPreviewPlanet() {
    previewPlanet = PlanetBuffer();
}

void Buffers::clearBuffers() {
    planetBuffers.clear();
}

void Buffers::setPreviewMode(bool preview) {
    isPreviewMode = preview;
}

bool Buffers::isInPreviewMode() const {
    return isPreviewMode;
}

/*
** Init
*/
void Buffers::init() {
    shaderController->initProgram();
    emscripten_log(EM_LOG_CONSOLE, "init buffers!");
}
