#pragma once
#include <glm/glm.hpp>
#include <unordered_map>

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

        bool aabb(
            glm::vec3 rayWorldDir,
            const glm::vec3& planetPosition,
            float planetSize
        );

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
            int planetIndex
        );
        bool isMouseIntersecting() const;
        bool checkIntersection(
            double mouseX,
            double mouseY,
            int viewportWidth,
            int viewportHeight,
            const glm::vec3& planetPosition,
            float planetSize,
            int planetIndex
        );

        bool handleClick(
            double x, 
            double y, 
            int viewportWidth, 
            int viewportHeight,
            const glm::vec3& planetPosition,
            float planetSize,
            int planetIndex
        );

        void setIsIntersecting(bool intersecting) {
            isIntersecting = intersecting;
            if(!intersecting) {
                selectedPlanetIndex = -1;
            }
        }
        int getSelectedPlanetIndex() const { return selectedPlanetIndex; }
        void clearSelection() { selectedPlanetIndex = -1; }
};