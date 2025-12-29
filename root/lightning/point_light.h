#pragma once
#include <glm/glm.hpp>
#include <vector>

class PointLight {
    public:
        std::vector<PointLight> pointLights;

        glm::vec3 position;
        glm::vec3 color;
        float intensity;
        float constant;
        float linear;
        float quadratic;
        float radius;

        void calcRadius();
        void add(const PointLight& pointLight);
        void remove(int i);
        void clear();
        const std::vector<PointLight>& get() const;
    private:
        PointLight();
        ~PointLight();
};