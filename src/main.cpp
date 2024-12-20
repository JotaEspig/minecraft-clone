#include <iostream>

#include "axolote/engine.hpp"

#include "minecraft/utils.hpp"

class App : public axolote::Window {
public:
    void main_loop();
};

void App::main_loop() {
    auto post_processing_shader = axolote::gl::Shader::create(
        my_get_path(
            "./resources/shaders/post_processing_base_vertex_shader.glsl"
        ),
        my_get_path(
            "./resources/shaders/post_processing_base_fragment_shader.glsl"
        )
    );
    std::shared_ptr<axolote::Scene> scene{new axolote::Scene{}};
    scene->renderer.init(width(), height());
    scene->renderer.setup_shader(post_processing_shader);
    set_scene(scene);

    float delta_t = 1.0f / 60.0f;
    while (!should_close()) {
        poll_events();
        process_input();

        update_camera((float)width() / height());
        update(delta_t);

        clear();
        render();

        flush();
    }
}

int main() {
    std::cout << "Axolote Engine" << std::endl;
    App app{};
    app.set_title("Window");
    app.set_window_size(600, 600);
    app.set_color(0x01, 0x40, 0xff);
    app.main_loop();
    return 0;
}
