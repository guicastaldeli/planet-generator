#pragma once
#include ".controller/shader_controller.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <emscripten.h>
#include "./.buffers/raycaster.h"

class Main;
class BufferController;
class Camera {
    private:
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

        glm::vec3 savedPosition;
        glm::vec3 savedTarget;
        bool panningLocked;
        bool rotationLocked;

        bool isFollowingPlanet;
        int followingPlanetIndex;
        glm::vec3 followingPlanetOffset;

        void updateVectors();
        void rotate(float deltaX, float deltaY);
        void pan(float deltaX, float deltaY);
        void zoom(float delta);
    public:
        Camera(
            Main* main, 
            ShaderController* shaderController,
            BufferController* bufferController
        );
        ~Camera();
        
        Main* main;
        Raycaster* raycaster;
        BufferController* bufferController;

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

        void setPosition(float x, float y, float z);
        void releaseCamera();
        
        void updateFollowing();
        void saveCurrentPos();
        void zoomToObj(const glm::vec3& planetPosition, float planetSize);
        void resetToSavedPos();
        void lockPanning(bool lock);
        void lockRotation(bool lock);
        bool isPanningLocked() const { return panningLocked; }

        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix();
};