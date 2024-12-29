#include <iostream>
#include <memory>

#include "axolote/gl/shader.hpp"
#include "axolote/glad/glad.h"
#include "axolote/scene.hpp"
#include "axolote/window.hpp"
#define DEBUG
#include "axolote/utils.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "minecraft/app.hpp"
#include "minecraft/frustum_cull.hpp"
#include "minecraft/utils.hpp"
#include "minecraft/world.hpp"

void App::process_input(double dt) {
    axolote::Window::process_input(dt);

    KeyState p_key_state = get_key_state(Key::P);
    if (p_key_state == KeyState::PRESSED && !is_key_pressed(Key::P)) {
        GLint next_polygon_mode = polygon_mode == GL_LINE ? GL_FILL : GL_LINE;
        glPolygonMode(GL_FRONT_AND_BACK, next_polygon_mode);
        set_key_pressed(Key::P, true);
        polygon_mode = next_polygon_mode;
    }
    else if (p_key_state == KeyState::RELEASED && is_key_pressed(Key::P)) {
        set_key_pressed(Key::P, false);
    }
}

void App::update(double dt) {
    World::frustum = Frustum{current_scene()->context->camera.matrix()};
    world->player_pos = current_scene()->context->camera.pos;

    Window::update(dt);
}

void App::main_loop() {
    auto post_processing_shader = axolote::gl::Shader::create(
        my_get_path("/resources/shaders/post_processing_base_vertex_shader.glsl"
        ),
        my_get_path(
            "/resources/shaders/post_processing_base_fragment_shader.glsl"
        )
    );
    auto face_shader = axolote::gl::Shader::create(
        my_get_path("/resources/shaders/face_vert.glsl"),
        my_get_path("/resources/shaders/face_frag.glsl")
    );

    std::shared_ptr<axolote::Scene> scene{new axolote::Scene{}};
    scene->renderer.init(width(), height());
    scene->renderer.setup_shader(post_processing_shader);
    scene->context->camera.sensitivity = 30000.0f;
    scene->context->camera.speed = 10.0f;
    set_scene(scene);

    // Obligatory to disable CULL_FACE for the chunk, it's strange, seems like
    // OpenGL understands the face's in the wrong way, because it's being
    // rotated on shaders.
    // glDisable(GL_CULL_FACE);
    glLineWidth(2.0f);

    world = std::make_shared<World>();
    world->render_distance = 8;
    for (std::int64_t i = -16; i < 16; ++i) {
        for (std::int64_t j = -16; j < 16; ++j) {
            world->load_chunk({i, -1, j});
        }
    }
    world->bind_shader(face_shader);
    scene->add_drawable(world);

    float delta_t = 1.0f / 60.0f;
    float before = get_time();
    float accumulated_frames = 0.0f;
    float delta_time_to_print_fps = 0.0f;
    while (!should_close()) {
        poll_events();

        float now = get_time();
        float dt = now - before;
        before = now;

        ++accumulated_frames;
        delta_time_to_print_fps += dt;
        if (delta_time_to_print_fps >= 2.0f) {
            axolote::debug(
                "FPS: %lf", accumulated_frames / delta_time_to_print_fps
            );
            accumulated_frames = 0.0f;
            delta_time_to_print_fps = 0.0f;
        }

        process_input(dt);

        update_camera((float)width() / height());
        update(delta_t);

        clear();
        render();

        imgui();

        flush();
    }
}

void App::imgui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(400, 100), ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls");

    ImGui::SliderFloat(
        "Mouse Sensitivity", &current_scene()->context->camera.sensitivity,
        0.0f, 100000.0f
    );
    ImGui::SliderFloat(
        "Camera Speed", &current_scene()->context->camera.speed, 0.0f, 100.0f
    );
    ImGui::InputInt("Render Distance", (int*)(&world->render_distance));

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
