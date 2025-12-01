#pragma once
#include "shader_controller.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <emscripten.h>
#include "raycaster.h"

class Main;
class Camera {
    private:
        Main* main;
        ShaderController* shaderController;

        float yaw;
        float pitch;

        bool isRotating;
        bool isPanning;
        double lastMouseX;
        double lastMouseY;

        float rotationSpeed;
        float panSpeed;
        float zoomSpeed;

        void updateVectors();
        void rotate(float deltaX, float deltaY);
        void pan(float deltaX, float deltaY);
        void zoom(float delta);
    public:
        Raycaster* raycaster;
        
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up;
        glm::vec3 right;
        float zoomLevel;

        void handleMouseDown(double x, double y, int button);
        void handleMouseUp(double x, double y, int button);
        void handleMouseMove(double x, double y);
        void handleMouseScroll(double delta);

        void reset();
        void set();
        void setEvents();
        void init();
        void update();
        Camera(Main* main, ShaderController* shaderController);
        ~Camera();
};