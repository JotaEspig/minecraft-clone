#pragma once

#include "axolote/glad/glad.h"
#include "axolote/window.hpp"

#include "minecraft/world.hpp"

class App : public axolote::Window {
public:
    GLint polygon_mode = GL_FILL;
    std::shared_ptr<World> world;

    void imgui();
    void process_input(double dt);
    void update(double dt);
    void main_loop();
};
