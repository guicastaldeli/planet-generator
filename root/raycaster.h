#pragma once
#include <glm/glm.hpp>

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

    bool aabb(glm::vec3 rayWorldDir);

public:
    Raycaster(
        Main* main,
        Camera* camera, 
        Buffers* buffers,
        ShaderController* shaderController
    );
    ~Raycaster();

    bool checkIntersection(
        double mouseX,
        double mouseY,
        int viewportWidth,
        int viewportHeight
    );
    void render(double x, double y);
    bool isMouseIntersecting() const;
};