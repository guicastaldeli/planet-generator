#include <stdio.h>
#include <GLFW/glfw3.h>
#include <emscripten.h>
#include "main.h"

class Camera {
    private:
        Main* main;

        void set();

    public:
        void init();
};