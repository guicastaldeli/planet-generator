#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

class PointLight {
    public:
        PointLight();
        PointLight(const PointLight& ot);
        PointLight& operator=(const PointLight& ot);
        ~PointLight();

        static std::vector<PointLight> pointLights;

        glm::vec3 position;
        glm::vec3 color;
        float intensity;
        float constant;
        float linear;
        float quadratic;
        float radius;

        int associatedPlanetId;
        std::string planetName;
        bool isSunLight;

        void calcRadius();
        void add(const PointLight& pointLight);
        void remove(int i);
        void clear();
        const std::vector<PointLight>& get() const;
};