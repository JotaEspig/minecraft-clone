#include <memory>
#include <utility>
#include <vector>

#include "axolote/gl/shader.hpp"
#include "axolote/gl/vao.hpp"
#include "axolote/gl/vbo.hpp"
#include "axolote/glad/glad.h"
#include <glm/glm.hpp>

#include "minecraft/atlas_mapping_uvs.hpp"
#include "minecraft/chunk.hpp"
#include "minecraft/face.hpp"
#include "minecraft/utils.hpp"

Chunk::Chunk() :
  Chunk{glm::vec3{0.0f}} {
}

Chunk::Chunk(const glm::vec3 &pos) :
  pos{pos} {
    if (atlas_texture == nullptr) {
        atlas_texture = axolote::gl::Texture::create(
            my_get_path("./resources/textures/atlas1.png"), "diffuse", (GLuint)0
        );
    }
    texture = atlas_texture;

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
        CHUNK_MAX_BLOCKS_AMOUNT * sizeof(glm::vec3), 0,
        GL_DYNAMIC_DRAW
    );
    vao->link_attrib(
        instanced_position_vbo, 1, 3, GL_FLOAT, sizeof(glm::vec3), (void *)0
    );
    vao->attrib_divisor(instanced_position_vbo, 1, 1);

    // Instanced Tex Coords
    instanced_tex_coord_vbo = axolote::gl::VBO::create();
    instanced_tex_coord_vbo->bind();
    instanced_tex_coord_vbo->buffer_data(
        CHUNK_MAX_BLOCKS_AMOUNT * sizeof(glm::vec4),
        0, GL_DYNAMIC_DRAW
    );
    vao->link_attrib(
        instanced_tex_coord_vbo, 2, 4, GL_FLOAT, sizeof(glm::vec4), (void *)0
    );
    vao->attrib_divisor(instanced_tex_coord_vbo, 2, 1);

    // Instanced Normals
    instanced_normal_vbo = axolote::gl::VBO::create();
    instanced_normal_vbo->bind();
    instanced_normal_vbo->buffer_data(
        CHUNK_MAX_BLOCKS_AMOUNT * sizeof(glm::vec3), 0,
        GL_DYNAMIC_DRAW
    );
    vao->link_attrib(
        instanced_normal_vbo, 3, 3, GL_FLOAT, sizeof(glm::vec3), (void *)0
    );
    vao->attrib_divisor(instanced_normal_vbo, 3, 1);

    vao->unbind();
    instanced_position_vbo->unbind();
    instanced_normal_vbo->unbind();
}

void Chunk::update_vbos() {
    glm::vec3 chunk_pos
        = glm::vec3{CHUNK_XZ_SIZE, CHUNK_Y_SIZE, CHUNK_XZ_SIZE} * pos;


    std::vector<glm::vec3> instanced_positions;
    std::vector<glm::vec4> instanced_tex_coords;
    std::vector<glm::vec3> instanced_normals;

    std::vector<std::pair<glm::vec3, Face>> drawable_faces
        = get_drawable_faces();
    for (const auto &[block_pos, face] : drawable_faces) {
        instanced_positions.push_back(
            chunk_pos + block_pos + face.normal * 0.5f
        );
        instanced_tex_coords.push_back(face.uv);
        instanced_normals.push_back(face.normal);
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
    instanced_normal_vbo->bind();
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, instanced_normals.size() * sizeof(glm::vec3),
        instanced_normals.data()
    );
    vao->unbind();
    instanced_position_vbo->unbind();
}

std::vector<std::pair<glm::vec3, Face>> Chunk::get_drawable_faces() const {
    std::vector<std::pair<glm::vec3, Face>> faces;
    for (int i = 0; i < CHUNK_XZ_SIZE; ++i) {
        for (int j = 0; j < CHUNK_Y_SIZE; ++j) {
            for (int k = 0; k < CHUNK_XZ_SIZE; ++k) {
                BlockType block = blocks[i][j][k];
                if (block == BlockType::AIR) {
                    continue;
                }
                glm::vec3 position = glm::vec3{i, j, k};
                for (int l = 0; l < Face::directions.size(); ++l) {
                    Face::Direction dir = Face::directions[l];
                    glm::vec3 normal = Face::get_normal_from_direction(dir);
                    bool is_facing_air = false;
                    int i2 = i + normal.x;
                    int j2 = j + normal.y;
                    int k2 = k + normal.z;
                    bool passed_array_bounds = i2 < 0 || i2 >= CHUNK_XZ_SIZE
                                               || j2 < 0 || j2 >= CHUNK_Y_SIZE
                                               || k2 < 0 || k2 >= CHUNK_XZ_SIZE;
                    if (passed_array_bounds) {
                        is_facing_air = true;
                    }
                    else {
                        is_facing_air
                            = blocks[i2][j2][k2] == BlockType::AIR;
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
