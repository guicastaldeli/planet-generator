#pragma once
#include "point_light.h"
#include <vector>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include "ambient_light.h"
#include "point_light.h"

class ShaderController;
class LightManager {
    public:
        LightManager(ShaderController* shaderController);
        ~LightManager();

        void setUniforms(GLuint shaderProgram);

        AmbientLight getAmbientLight();
        PointLight getPointLight();
    private:
        AmbientLight* ambientLight;
        PointLight* pointLight;
}