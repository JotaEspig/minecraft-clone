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

constexpr std::size_t CHUNK_SIZE = 16;
constexpr std::size_t CHUNK_MAX_BLOCKS_AMOUNT
    = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

// typedef a type for a chunk (3D grid of blocks based on CHUNK_SIZE)

class Chunk : public axolote::Drawable {
public:
    // coordinates in integer, x=1 means x=1 * CHUNK_SIZE, y=1 means y=1 *
    // CHUNK_SIZE, z=1 means z=1 * CHUNK_SIZE
    class Coord {
    public:
        struct Compare {
            bool operator()(const Coord &a, const Coord &b) const;
        };

        std::int64_t x, y, z;
        Coord(std::int64_t x, std::int64_t y, std::int64_t z);
        bool operator==(const Coord &other) const;
    };

    typedef std::array<
        std::array<std::array<BlockType, CHUNK_SIZE>, CHUNK_SIZE>,
        CHUNK_SIZE>
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

    glm::vec3 pos{0.0f};
    long long faces_to_draw_amount = 0;
    bool has_changed = true;
    Grid blocks;

    Chunk();
    Chunk(const Coord &coord);

    void update_vbos();
    std::vector<std::pair<glm::vec3, Face>> get_drawable_faces() const;

    void bind_shader(std::shared_ptr<axolote::gl::Shader> shader) override;
    std::vector<std::shared_ptr<axolote::gl::Shader>>
    get_shaders() const override;
    void update(double dt) override;
    void draw() override;
    void draw(const glm::mat4 &mat) override;
};

template <>
struct std::hash<Chunk::Coord> {
    std::size_t operator()(const Chunk::Coord &coord) const {
        return std::hash<std::int64_t>()(coord.x)
               ^ std::hash<std::int64_t>()(coord.y)
               ^ std::hash<std::int64_t>()(coord.z);
    }
};
