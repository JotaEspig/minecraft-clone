#pragma once

#include <string>
#include <array>

#include "axolote/gl/ebo.hpp"
#include "axolote/gl/vao.hpp"
#include "axolote/gl/vbo.hpp"
#include "axolote/gl/texture.hpp"
#include <glm/glm.hpp>

#include "minecraft/atlas_mapping_uvs.hpp"

class Face {
public:
    enum class Direction { TOP = 0, BOTTOM, LEFT, RIGHT, FRONT, BACK, INVALID };

    static const std::vector<glm::vec3> vertices;
    static const std::vector<GLuint> indices;
    static const std::array<Direction, 6> directions;

    Direction dir = Direction::INVALID;
    glm::vec4 uv{0.0f};

    Face();
    Face(BlockType type, Direction dir);

    static glm::vec3 get_normal_from_direction(Direction dir);
};
