#include <iostream>

#include "axolote/gl/shader.hpp"
#include "axolote/scene.hpp"
#include "axolote/window.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "minecraft/atlas_mapping_uvs.hpp"
#include "minecraft/chunk.hpp"
#include "minecraft/utils.hpp"

class App : public axolote::Window {
public:
    void process_input(double dt);
    void main_loop();
};

void App::process_input(double dt) {
    axolote::Window::process_input(dt);

    KeyState p_key_state = get_key_state(Key::P);
    if (p_key_state == KeyState::PRESSED) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (p_key_state == KeyState::RELEASED) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void App::main_loop() {
    // Setup atlas mapping uvs
    atlas_mapping_uvs = read_from_file();

    auto post_processing_shader = axolote::gl::Shader::create(
        my_get_path(
            "./resources/shaders/post_processing_base_vertex_shader.glsl"
        ),
        my_get_path(
            "./resources/shaders/post_processing_base_fragment_shader.glsl"
        )
    );
    auto face_shader = axolote::gl::Shader::create(
        my_get_path("./resources/shaders/face_vert.glsl"),
        my_get_path("./resources/shaders/face_frag.glsl")
    );
    auto object3d_shader = axolote::gl::Shader::create(
        my_get_path("./resources/shaders/object3d_base_vertex_shader.glsl"),
        my_get_path("./resources/shaders/object3d_base_fragment_shader.glsl")
    );

    std::shared_ptr<axolote::Scene> scene{new axolote::Scene{}};
    scene->renderer.init(width(), height());
    scene->renderer.setup_shader(post_processing_shader);
    scene->context->camera.sensitivity = 10000.0f;
    scene->context->camera.speed = 10.0f;
    set_scene(scene);

    auto chunk = std::make_shared<Chunk>();
    chunk->bind_shader(face_shader);
    scene->add_drawable(chunk);

    std::vector<Block> blocks;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            for (int k = 0; k < 16; k++) {
                Block block{glm::vec3{i, j, k}, "grass"};
                blocks.push_back(block);
            }
        }
    }

    chunk->blocks = blocks;
    chunk->update_vbos();

    glDisable(GL_CULL_FACE);

    float delta_t = 1.0f / 60.0f;
    float before = get_time();
    while (!should_close()) {
        poll_events();

        float now = get_time();
        float dt = now - before;
        before = now;

        process_input(dt);

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
