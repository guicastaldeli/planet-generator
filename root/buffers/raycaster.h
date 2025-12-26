#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
#include "buffer_data.h"

class Main;
class Camera;
class Buffers;
class ShaderController;
class Raycaster {
    private:
        Main* main;
        Camera* camera;
        Buffers* buffers;
        ShaderController* shaderController;
        bool isIntersecting;

        std::unordered_map<int, bool> planetIntersections;

    public:
        Raycaster(
            Main* main,
            Camera* camera, 
            Buffers* buffers,
            ShaderController* shaderController
        );
        ~Raycaster();

        int selectedPlanetIndex;

        void render(
            double x, 
            double y,
            const glm::vec3& planetPosition,
            float planetSize,
            int planetIndex,
            BufferData::Type shapeType
        );
        bool isMouseIntersecting() const;
        bool checkIntersection(
            double mouseX,
            double mouseY,
            int viewportWidth,
            int viewportHeight,
            const glm::vec3& planetPosition,
            float planetSize,
            int planetIndex,
            BufferData::Type shapeType
        );

        bool handleClick(
            double x, 
            double y, 
            int viewportWidth, 
            int viewportHeight,
            const glm::vec3& planetPosition,
            float planetSize,
            int planetIndex,
            BufferData::Type shapeType
        );

        void setIsIntersecting(bool intersecting) {
            isIntersecting = intersecting;
            if(!intersecting) {
                selectedPlanetIndex = -1;
            }
        }
        int getSelectedPlanetIndex() const { return selectedPlanetIndex; }
        void clearSelection() { selectedPlanetIndex = -1; }

        bool sphereIntersection(
            glm::vec3 rayWorldDir,
            const glm::vec3& planetPosition,
            float planetSize
        );
        bool cubeIntersection(
            glm::vec3 rayWorldDir,
            const glm::vec3& planetPosition,
            float planetSize
        );
        bool triangleIntersection(
            glm::vec3 rayWorldDir,
            const glm::vec3& planetPosition,
            float planetSize
        );
        bool meshIntersection(
            glm::vec3 rayWorldDir,
            const glm::vec3& planetPosition,
            float planetSize,
            BufferData::Type shapeType
        );
        bool rayTriangleIntersection(
            const glm::vec3& rayOrigin,
            const glm::vec3& rayDir,
            const glm::vec3& v0,
            const glm::vec3& v1,
            const glm::vec3& v2
        );
};