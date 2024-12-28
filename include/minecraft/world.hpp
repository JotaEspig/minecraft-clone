#pragma once

#include <map>
#include <memory>

#include "axolote/drawable.hpp"

#include "minecraft/chunk.hpp"
#include "minecraft/frustum_cull.hpp"

class World : public axolote::Drawable {
public:
    static Frustum frustum;

    glm::vec3 player_pos{0.0f};
    std::uint16_t render_distance = 6;
    std::map<Chunk::Coord, std::shared_ptr<Chunk>, Chunk::Coord::Compare>
        chunks;

    World();
    World(std::uint16_t render_distance);

    void load_chunk(const Chunk::Coord &coord);
    // calculate the distance between the player and the chunk using only the x
    // and z coordinates because the render distance is based on the x and z
    float distance_to_player(const glm::vec3 &chunk_pos) const;

    void bind_shader(std::shared_ptr<axolote::gl::Shader> shader) override;
    std::vector<std::shared_ptr<axolote::gl::Shader>>
    get_shaders() const override;
    void update(double dt) override;
    void draw() override;
    void draw(const glm::mat4 &mat) override;
};