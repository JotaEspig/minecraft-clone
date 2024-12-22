#include "minecraft/chunk.hpp"
#include "minecraft/face.hpp"

Chunk::Chunk() {
    Face::init();

    Face::vao->bind();

    // Instanced Positions
    instanced_position_vbo = axolote::gl::VBO::create();
    instanced_position_vbo->bind();
    instanced_position_vbo->buffer_data(
        CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * sizeof(glm::vec3),
        instanced_positions.data(), GL_DYNAMIC_DRAW
    );
    Face::vao->link_attrib(
        instanced_position_vbo, 1, 3, GL_FLOAT, sizeof(glm::vec3), (void *)0
    );
    Face::vao->attrib_divisor(instanced_position_vbo, 1, 1);

    // Instanced Tex Coords
    instanced_tex_coord_vbo = axolote::gl::VBO::create();
    instanced_tex_coord_vbo->bind();
    instanced_tex_coord_vbo->buffer_data(
        CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * sizeof(glm::vec4),
        instanced_tex_coords.data(), GL_DYNAMIC_DRAW
    );
    Face::vao->link_attrib(
        instanced_tex_coord_vbo, 2, 4, GL_FLOAT, sizeof(glm::vec4), (void *)0
    );
    Face::vao->attrib_divisor(instanced_tex_coord_vbo, 2, 1);

    // Instanced Normals
    instanced_normal_vbo = axolote::gl::VBO::create();
    instanced_normal_vbo->bind();
    instanced_normal_vbo->buffer_data(
        CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * sizeof(glm::vec3),
        instanced_normals.data(), GL_DYNAMIC_DRAW
    );
    Face::vao->link_attrib(
        instanced_normal_vbo, 3, 3, GL_FLOAT, sizeof(glm::vec3), (void *)0
    );
    Face::vao->attrib_divisor(instanced_normal_vbo, 3, 1);

    Face::vao->unbind();
    instanced_position_vbo->unbind();
    instanced_normal_vbo->unbind();
}

void Chunk::update_vbos() {
    instanced_positions.clear();
    instanced_normals.clear();
    for (auto &block : blocks) {
        if (block.type == "air") {
            continue;
        }
        for (auto face : block.faces) {
            instanced_positions.push_back(block.position + face.normal);
            instanced_tex_coords.push_back(face.uv);
            instanced_normals.push_back(face.normal);
        }
    }

    Face::vao->bind();
    instanced_position_vbo->bind();
    instanced_position_vbo->buffer_data(
        instanced_positions.size() * sizeof(glm::vec3),
        instanced_positions.data(), GL_DYNAMIC_DRAW
    );
    instanced_tex_coord_vbo->bind();
    instanced_tex_coord_vbo->buffer_data(
        instanced_tex_coords.size() * sizeof(glm::vec4),
        instanced_tex_coords.data(), GL_DYNAMIC_DRAW
    );
    instanced_normal_vbo->bind();
    instanced_normal_vbo->buffer_data(
        instanced_normals.size() * sizeof(glm::vec3), instanced_normals.data(),
        GL_DYNAMIC_DRAW
    );
    Face::vao->unbind();
    instanced_position_vbo->unbind();
}

void Chunk::bind_shader(std::shared_ptr<axolote::gl::Shader> shader) {
    Chunk::shader = shader;
}

std::vector<std::shared_ptr<axolote::gl::Shader>> Chunk::get_shaders() const {
    return {Chunk::shader};
}

void Chunk::update(double dt) {
}

void Chunk::draw() {
    shader->use();
    Face::texture->bind();
    Face::texture->activate();
    shader->set_uniform_int("texture0", Face::texture->unit());
    Face::vao->bind();
    glDrawElementsInstanced(
        GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instanced_positions.size()
    );
    Face::vao->unbind();
    Face::texture->unbind();
}

void Chunk::draw(const glm::mat4 &mat) {
    (void)mat;
    draw();
}
