#include "light_manager.h"
#include "../_shaders/shader_controller.h"
#include "../buffers/buffer_generator.h"

LightManager::LightManager(ShaderController* shaderController) :
    shaderController(nullptr)
{
    shaderController = shaderController;
    ambientLight = new AmbientLight();
    pointLight = new PointLight();
};
LightManager::~LightManager() {};

/**
 * Set Uniforms
 */
void LightManager::setUniforms(GLuint shaderProgram) {
    if(ambientLight->isEnabled()) {
        GLuint ambientColorLoc = glGetUniformLocation(shaderProgram, "uAmbientLight.color");
        GLuint ambientIntensityLoc = glGetUniformLocation(shaderProgram, "uAmbientLight.intensity");
        GLuint ambientLightEnabledLoc = glGetUniformLocation(shaderProgram, "uAmbientLight.enabled");

        if(ambientColorLoc != -1) {
            glm::vec3 color = ambientLight->getColor();
            glUniform3f(ambientColorLoc, color.r, color.g, color.b);
        }
        if(ambientIntensityLoc != -1) {
            glUniform1f(ambientIntensityLoc, ambientLight->getIntensity());
        }
        if(ambientLightEnabledLoc != -1) {
            glUniform1i(ambientLightEnabledLoc, ambientLight->isEnabled() ? 1 : 0);
        }
    }

    int visibleLightCount = 0;
    for(const auto& light : pointLight->pointLights) {
        if(!light.isHidden) {
            visibleLightCount++;
        }
    }

    GLuint numLightsLoc = glGetUniformLocation(shaderProgram, "uNumPointLights");
    if(numLightsLoc != -1) glUniform1i(numLightsLoc, visibleLightCount);
    
    int visibleIndex = 0;
    for(size_t i = 0; i < pointLight->pointLights.size(); i++) {
        const PointLight& light = pointLight->pointLights[i];
        if(light.isHidden) continue;

         std::string baseName = "uPointLights[" + std::to_string(visibleIndex) + "]";

        GLuint posLoc = glGetUniformLocation(shaderProgram, (baseName + ".position").c_str());
        if(posLoc != -1) glUniform3f(posLoc, light.position.x, light.position.y, light.position.z);

        GLuint colorLoc = glGetUniformLocation(shaderProgram, (baseName + ".color").c_str());
        if(colorLoc != -1) glUniform3f(colorLoc, light.color.r, light.color.g, light.color.b);

        GLint intensityLoc = glGetUniformLocation(shaderProgram, (baseName + ".intensity").c_str());
        if(intensityLoc != -1) glUniform1f(intensityLoc, light.intensity);
            
        GLint constantLoc = glGetUniformLocation(shaderProgram, (baseName + ".constant").c_str());
        if(constantLoc != -1) glUniform1f(constantLoc, light.constant);
            
        GLint linearLoc = glGetUniformLocation(shaderProgram, (baseName + ".linear").c_str());
        if(linearLoc != -1) glUniform1f(linearLoc, light.linear);
            
        GLint quadraticLoc = glGetUniformLocation(shaderProgram, (baseName + ".quadratic").c_str());
        if(quadraticLoc != -1) glUniform1f(quadraticLoc, light.quadratic);
            
        GLint radiusLoc = glGetUniformLocation(shaderProgram, (baseName + ".radius").c_str());
        if(radiusLoc != -1) glUniform1f(radiusLoc, light.radius);

        visibleIndex++;
    }
}

/**
 * Update Planet Lights
 */
void LightManager::updatePlanetLights(const std::vector<PlanetBuffer>& planetBuffers) {
    for(auto& pointLight : pointLight->pointLights) {
        if(pointLight.isHidden) continue;

        for(const auto& planetBuffer : planetBuffers) {
            if(planetBuffer.data.id == pointLight.associatedPlanetId) {
                pointLight.position = planetBuffer.worldPos;
                break;
            }
        }
    }
}

/**
 * Get Ambient Light
 */
AmbientLight* LightManager::getAmbientLight() {
    return ambientLight;
}

/**
 * Get Point Light
 */
PointLight* LightManager::getPointLight() {
    return pointLight;
}