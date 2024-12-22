#pragma once

#include "axolote/drawable.hpp"
#include "axolote/gl/shader.hpp"
#include "axolote/gl/vbo.hpp"

#include "minecraft/block.hpp"

#define CHUNK_SIZE 16

class Chunk : public axolote::Drawable {
public:
    // Shader
    std::shared_ptr<axolote::gl::Shader> shader;
    glm::vec3 pos;
    // Instanced Postion
    std::shared_ptr<axolote::gl::VBO> instanced_position_vbo;
    std::vector<glm::vec3> instanced_positions;
    // Instanced Tex Coord
    std::shared_ptr<axolote::gl::VBO> instanced_tex_coord_vbo;
    std::vector<glm::vec4> instanced_tex_coords;
    // Instanced normal
    std::shared_ptr<axolote::gl::VBO> instanced_normal_vbo;
    std::vector<glm::vec3> instanced_normals;

    std::vector<Block> blocks;

    Chunk();

    void update_vbos();

    void bind_shader(std::shared_ptr<axolote::gl::Shader> shader) override;
    std::vector<std::shared_ptr<axolote::gl::Shader>>
    get_shaders() const override;
    void update(double dt) override;
    void draw() override;
    void draw(const glm::mat4 &mat) override;
};
