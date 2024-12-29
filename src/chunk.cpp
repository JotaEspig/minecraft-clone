#include <memory>
#include <utility>
#include <vector>

#include "axolote/gl/shader.hpp"
#include "axolote/gl/vao.hpp"
#include "axolote/gl/vbo.hpp"
#include "axolote/glad/glad.h"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include "minecraft/atlas_mapping_uvs.hpp"
#include "minecraft/chunk.hpp"
#include "minecraft/face.hpp"
#include "minecraft/utils.hpp"

std::shared_ptr<axolote::gl::Texture> Chunk::texture = nullptr;
std::shared_ptr<axolote::gl::Shader> Chunk::shader = nullptr;

bool Chunk::Coord::Compare::operator()(
    const Chunk::Coord &a, const Chunk::Coord &b
) const {
    glm::vec3 vec_a = glm::vec3{a.x, a.y, a.z};
    glm::vec3 vec_b = glm::vec3{b.x, b.y, b.z};
    float length_a = glm::length2(vec_a);
    float length_b = glm::length2(vec_b);
    if (length_a != length_b) {
        return length_a < length_b;
    }

    // Secondary criteria: Compare x, y, and z
    if (a.x != b.x) {
        return a.x < b.x;
    }
    if (a.y != b.y) {
        return a.y < b.y;
    }
    return a.z < b.z;
}

Chunk::Coord::Coord(std::int64_t x, std::int64_t y, std::int64_t z) :
  x{x},
  y{y},
  z{z} {
}

bool Chunk::Coord::operator==(const Coord &other) const {
    return x == other.x && y == other.y && z == other.z;
}

Chunk::Chunk() :
  Chunk{Coord{0, 0, 0}} {
}

Chunk::Chunk(const Coord &coord) {
    if (atlas_texture == nullptr) {
        atlas_texture = axolote::gl::Texture::create(
            my_get_path("./resources/textures/atlas1.png"), "diffuse", (GLuint)0
        );
        texture = atlas_texture;
    }

    pos = glm::vec3{coord.x, coord.y, coord.z};
    pos *= glm::vec3{CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE};

    vao = axolote::gl::VAO::create();
    vao->bind();

    vbo = axolote::gl::VBO::create();
    vbo->bind();
    vbo->buffer_data(
        Face::vertices.size() * sizeof(glm::vec3), Face::vertices.data()
    );
    ebo = axolote::gl::EBO::create(Face::indices);
    ebo->bind();

    vbo->bind();
    vao->link_attrib(vbo, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void *)0);

    // Instanced Positions
    instanced_position_vbo = axolote::gl::VBO::create();
    instanced_position_vbo->bind();
    instanced_position_vbo->buffer_data(
        CHUNK_MAX_BLOCKS_AMOUNT * sizeof(glm::vec3), 0, GL_DYNAMIC_DRAW
    );
    vao->link_attrib(
        instanced_position_vbo, 1, 3, GL_FLOAT, sizeof(glm::vec3), (void *)0
    );
    vao->attrib_divisor(instanced_position_vbo, 1, 1);

    // Instanced Tex Coords
    instanced_tex_coord_vbo = axolote::gl::VBO::create();
    instanced_tex_coord_vbo->bind();
    instanced_tex_coord_vbo->buffer_data(
        CHUNK_MAX_BLOCKS_AMOUNT * sizeof(glm::vec4), 0, GL_DYNAMIC_DRAW
    );
    vao->link_attrib(
        instanced_tex_coord_vbo, 2, 4, GL_FLOAT, sizeof(glm::vec4), (void *)0
    );
    vao->attrib_divisor(instanced_tex_coord_vbo, 2, 1);

    // Instanced Normals
    instanced_direction_vbo = axolote::gl::VBO::create();
    instanced_direction_vbo->bind();
    instanced_direction_vbo->buffer_data(
        CHUNK_MAX_BLOCKS_AMOUNT * sizeof(int), 0, GL_DYNAMIC_DRAW
    );
    vao->link_attrib(
        instanced_direction_vbo, 3, 1, GL_FLOAT, sizeof(int), (void *)0
    );
    vao->attrib_divisor(instanced_direction_vbo, 3, 1);

    vao->unbind();
    instanced_position_vbo->unbind();
    instanced_direction_vbo->unbind();
}

void Chunk::update_vbos() {
    std::vector<glm::vec3> instanced_positions;
    std::vector<glm::vec4> instanced_tex_coords;
    std::vector<int> instanced_directions;

    std::vector<std::pair<glm::vec3, Face>> drawable_faces
        = get_drawable_faces();
    for (const auto &[block_pos, face] : drawable_faces) {
        instanced_positions.push_back(
            pos + block_pos + Face::get_normal_from_direction(face.dir) * 0.5f
        );
        instanced_tex_coords.push_back(face.uv);
        instanced_directions.push_back(static_cast<int>(face.dir));
    }

    faces_to_draw_amount = instanced_positions.size();

    vao->bind();
    instanced_position_vbo->bind();
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, instanced_positions.size() * sizeof(glm::vec3),
        instanced_positions.data()
    );
    instanced_tex_coord_vbo->bind();
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, instanced_tex_coords.size() * sizeof(glm::vec4),
        instanced_tex_coords.data()
    );
    instanced_direction_vbo->bind();
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, instanced_directions.size() * sizeof(int),
        instanced_directions.data()
    );
    vao->unbind();
    instanced_position_vbo->unbind();
}

std::vector<std::pair<glm::vec3, Face>> Chunk::get_drawable_faces() const {
    std::vector<std::pair<glm::vec3, Face>> faces;
    for (std::size_t i = 0; i < CHUNK_SIZE; ++i) {
        for (std::size_t j = 0; j < CHUNK_SIZE; ++j) {
            for (std::size_t k = 0; k < CHUNK_SIZE; ++k) {
                BlockType block = blocks[i][j][k];
                if (block == BlockType::AIR) {
                    continue;
                }
                glm::vec3 position = glm::vec3{i, j, k};
                for (std::size_t l = 0; l < Face::directions.size(); ++l) {
                    Face::Direction dir = Face::directions[l];
                    glm::vec3 normal = Face::get_normal_from_direction(dir);
                    bool is_facing_air = false;
                    int i2 = i + normal.x;
                    int j2 = j + normal.y;
                    int k2 = k + normal.z;
                    bool passed_array_bounds = i2 < 0 || i2 >= CHUNK_SIZE
                                               || j2 < 0 || j2 >= CHUNK_SIZE
                                               || k2 < 0 || k2 >= CHUNK_SIZE;
                    if (passed_array_bounds) {
                        is_facing_air = true;
                    }
                    else {
                        is_facing_air = blocks[i2][j2][k2] == BlockType::AIR;
                    }
                    if (is_facing_air) {
                        Face face = Face{block, dir};
                        faces.push_back({position, face});
                    }
                }
            }
        }
    }
    return faces;
}

void Chunk::bind_shader(std::shared_ptr<axolote::gl::Shader> shader) {
    Chunk::shader = shader;
}

std::vector<std::shared_ptr<axolote::gl::Shader>> Chunk::get_shaders() const {
    return {Chunk::shader};
}

void Chunk::update(double dt) {
    if (has_changed) {
        update_vbos();
        has_changed = false;
    }
}

void Chunk::draw() {
    shader->use();
    texture->bind();
    texture->activate();
    shader->set_uniform_int("texture0", texture->unit());
    vao->bind();
    glDrawElementsInstanced(
        GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, faces_to_draw_amount
    );
    vao->unbind();
    texture->unbind();
}

void Chunk::draw(const glm::mat4 &mat) {
    (void)mat;
    draw();
}
