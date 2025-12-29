#pragma once
#include "point_light.h"
#include "ambient_light.h"
#include "point_light.h"
#include "../buffers/buffer_generator.h"
#include <vector>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>

class ShaderController;
class LightManager {
    public:
        LightManager(ShaderController* shaderController);
        ~LightManager();

        void setUniforms(GLuint shaderProgram);
        void updatePlanetLights(const std::vector<PlanetBuffer>& planetBuffers);

        AmbientLight* getAmbientLight();
        PointLight* getPointLight();
    private:
        AmbientLight* ambientLight;
        PointLight* pointLight;
        ShaderController* shaderController;
};