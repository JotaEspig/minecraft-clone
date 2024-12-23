#pragma once

#include <string>
#include <array>

#include <glm/glm.hpp>

#include "minecraft/face.hpp"
#include "minecraft/atlas_mapping_uvs.hpp"

class Block {
public:
    BlockType type;

    std::array<Face::Direction, 6> faces_directions;

    Block();
    Block(BlockType type);
};
