#pragma once

#include <array>
#include <memory>
#include <utility>
#include <vector>

#include "axolote/drawable.hpp"
#include "axolote/gl/shader.hpp"
#include "axolote/gl/vbo.hpp"
#include <glm/glm.hpp>

#include "minecraft/block.hpp"

constexpr unsigned long CHUNK_XZ_SIZE = 16;
constexpr unsigned long CHUNK_Y_SIZE = 256;
constexpr unsigned long long CHUNK_MAX_BLOCKS_AMOUNT
    = CHUNK_XZ_SIZE * CHUNK_Y_SIZE * CHUNK_XZ_SIZE;

// typedef a type for a chunk (3D grid of blocks based on CHUNK_XZ_SIZE)

class Chunk : public axolote::Drawable {
public:
    typedef std::array<
        std::array<std::array<Block, CHUNK_XZ_SIZE>, CHUNK_Y_SIZE>, CHUNK_XZ_SIZE>
        Grid;

    // VAO
    std::shared_ptr<axolote::gl::VAO> vao;
    // VBO
    std::shared_ptr<axolote::gl::VBO> vbo;
    // EBO
    std::shared_ptr<axolote::gl::EBO> ebo;
    // Texture
    std::shared_ptr<axolote::gl::Texture> texture;
    // Shader
    std::shared_ptr<axolote::gl::Shader> shader;
    // Instanced Postion
    std::shared_ptr<axolote::gl::VBO> instanced_position_vbo;
    // Instanced Tex Coord
    std::shared_ptr<axolote::gl::VBO> instanced_tex_coord_vbo;
    // Instanced normal
    std::shared_ptr<axolote::gl::VBO> instanced_normal_vbo;

    std::vector<glm::vec3> instanced_positions;
    std::vector<glm::vec4> instanced_tex_coords;
    std::vector<glm::vec3> instanced_normals;

    // This vec will be multiplied by CHUNK_XZ_SIZE, so if pos = {1, 0, 0}, the
    // actual chunk position will be {CHUNK_XZ_SIZE, 0, 0}
    glm::vec3 pos{0.0f};
    bool has_changed = true;
    Grid blocks;

    Chunk();
    Chunk(const glm::vec3 &pos);

    void update_vbos();
    std::vector<std::pair<glm::vec3, Face>> get_drawable_faces() const;

    void bind_shader(std::shared_ptr<axolote::gl::Shader> shader) override;
    std::vector<std::shared_ptr<axolote::gl::Shader>>
    get_shaders() const override;
    void update(double dt) override;
    void draw() override;
    void draw(const glm::mat4 &mat) override;
};
