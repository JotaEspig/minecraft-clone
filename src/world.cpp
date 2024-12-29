#include <iostream>
#include <memory>
#include <random>

#include "axolote/gl/shader.hpp"

#include "minecraft/chunk.hpp"
#include "minecraft/world.hpp"

Frustum World::frustum{};

World::World() {
}

World::World(std::uint16_t render_distance) :
  render_distance{render_distance} {
}

void World::load_chunk(const Chunk::Coord &coord) {
    auto c = chunks.find(coord);
    bool found = false;
    for (; c != chunks.end(); ++c) {
        if (c->first == coord) {
            found = true;
            break;
        }
    }
    if (found) {
        std::cerr << "Chunk already loaded" << std::endl;
        std::cerr << coord.x << " " << coord.y << " " << coord.z << std::endl;
        std::cerr << c->first.x << " " << c->first.y << " " << c->first.z
                  << std::endl;

        return;
    }

    std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(coord);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (std::size_t i = 0; i < CHUNK_SIZE; ++i) {
        for (std::size_t j = 0; j < CHUNK_SIZE; ++j) {
            for (std::size_t k = 0; k < CHUNK_SIZE; ++k) {
                BlockType block = BlockType::GRASS;
                if (j > CHUNK_SIZE * 2 / 3 && dis(gen) > 0.5) {
                    block = BlockType::AIR;
                }
                chunk->blocks[i][j][k] = block;
            }
        }
    }
    chunks[coord] = chunk;
}

float World::distance_to_player(const glm::vec3 &chunk_pos) const {
    glm::vec2 player_pos_xz = glm::vec2{player_pos.x, player_pos.z};
    glm::vec2 chunk_pos_xz = glm::vec2{chunk_pos.x, chunk_pos.z};
    return glm::distance(player_pos_xz, chunk_pos_xz);
}

void World::bind_shader(std::shared_ptr<axolote::gl::Shader> shader) {
    Chunk::shader = shader;
}

std::vector<std::shared_ptr<axolote::gl::Shader>> World::get_shaders() const {
    return {Chunk::shader};
}

void World::update(double dt) {
    for (auto &chunk : chunks) {
        chunk.second->update(dt);
    }
}

void World::draw() {
    for (auto &chunk : chunks) {
        // uses middle position of the chunk to calculate the distance
        // because chunk.pos is at the start of the chunk, so sometimes it
        // causes some inconsistency. So middle pos is there to improve the
        // accuracy of the render distance limit
        glm::vec3 middle_pos_of_chunk
            = chunk.second->pos
              + glm::vec3{CHUNK_SIZE / 2, CHUNK_SIZE / 2, CHUNK_SIZE / 2};
        float distance = distance_to_player(middle_pos_of_chunk);
        if (distance > (render_distance * CHUNK_SIZE)) {
            continue;
        }

        glm::vec3 minp = chunk.second->pos;
        glm::vec3 maxp = minp + glm::vec3{CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE};

        // distance < 2 ensures that the closest chunks to the players is
        // rendered to avoid visual bugs
        if (distance < 2 || frustum.IsBoxVisible(minp, maxp)) {
            chunk.second->draw();
        }
    }
}

void World::draw(const glm::mat4 &mat) {
    (void)mat;
    draw();
}
