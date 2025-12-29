#pragma once
#include "point_light.h"
#include <vector>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include "point_light.h"

class ShaderController;
class LightManager {
    public:
        LightManager(ShaderController* shaderController);
        ~LightManager();

        void setUniforms(GLuint shaderProgram);
        PointLight getPointLight();
    private:
        PointLight* pointLight;
}