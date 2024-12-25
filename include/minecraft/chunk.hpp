#pragma once

#include <array>
#include <memory>
#include <utility>
#include <vector>

#include "axolote/drawable.hpp"
#include "axolote/gl/shader.hpp"
#include "axolote/gl/vbo.hpp"
#include <glm/glm.hpp>

#include "minecraft/face.hpp"
#include "minecraft/frustum_cull.hpp"

constexpr std::size_t CHUNK_XZ_SIZE = 16;
constexpr std::size_t CHUNK_Y_SIZE = 16;
constexpr std::size_t CHUNK_MAX_BLOCKS_AMOUNT
    = CHUNK_XZ_SIZE * CHUNK_Y_SIZE * CHUNK_XZ_SIZE;

// typedef a type for a chunk (3D grid of blocks based on CHUNK_XZ_SIZE)

class Chunk : public axolote::Drawable {
public:
    typedef std::array<
        std::array<std::array<BlockType, CHUNK_XZ_SIZE>, CHUNK_Y_SIZE>, CHUNK_XZ_SIZE>
        Grid;

    // VAO
    std::shared_ptr<axolote::gl::VAO> vao;
    // VBO
    std::shared_ptr<axolote::gl::VBO> vbo;
    // EBO
    std::shared_ptr<axolote::gl::EBO> ebo;
    // Texture
    static std::shared_ptr<axolote::gl::Texture> texture;
    // Shader
    static std::shared_ptr<axolote::gl::Shader> shader;
    // Instanced Postion
    std::shared_ptr<axolote::gl::VBO> instanced_position_vbo;
    // Instanced Tex Coord
    std::shared_ptr<axolote::gl::VBO> instanced_tex_coord_vbo;
    // Instanced normal
    std::shared_ptr<axolote::gl::VBO> instanced_direction_vbo;

    static Frustum frustum;

    glm::vec3 pos{0.0f};
    long long faces_to_draw_amount = 0;
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
