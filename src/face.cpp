#include <memory>
#include <string>
#include <vector>

#include "axolote/gl/Texture.hpp"
#include "axolote/gl/ebo.hpp"
#include "axolote/gl/vao.hpp"
#include "axolote/gl/vbo.hpp"
#include "axolote/glad/glad.h"
#include <glm/glm.hpp>

#include "minecraft/atlas_mapping_uvs.hpp"
#include "minecraft/face.hpp"
#include "minecraft/utils.hpp"

// Pos, Color (vec4), Normal, TexCoord
const std::vector<glm::vec3> Face::vertices = {
    // 0.5 so each face has a length of 1
    // clang-format off
    {-0.5f, -0.5f, 0.0f},
    {0.5f, -0.5f, 0.0f},
    {0.5f, 0.5f, 0.0f},
    {-0.5f, 0.5f, 0.0f},
    // clang-format on
};

const std::vector<GLuint> Face::indices = {
    0, 1, 2, 2, 3, 0,
};

Face::Face() {
}

Face::Face(BlockType type, Direction dir) {
    if (type == BlockType::AIR)
        return;

    int index = static_cast<int>(type) + static_cast<int>(dir);
    glm::vec4 uv_for_type = get_atlas_mapping_uvs().at(index);
    normal = get_normal_from_direction(dir);

    float width = uv_for_type.z;
    float height = uv_for_type.w;
    float x1 = uv_for_type.x;
    float x2 = uv_for_type.x + width;
    // AtlasGenerator has origin at top left, OpenGL has origin at bottom left,
    // that's why we need to flip the y axis and subtract the height to get the
    // correct first y (where the coordinate starts)
    float y1 = 1.0f - uv_for_type.y - height;
    float y2 = 1.0f - uv_for_type.y;
    uv = {x1, y1, x2, y2};
}

glm::vec3 Face::get_normal_from_direction(Direction dir) {
    switch (dir) {
    case Direction::TOP:
        return glm::vec3{0.0f, 1.0f, 0.0f};
    case Direction::BOTTOM:
        return glm::vec3{0.0f, -1.0f, 0.0f};
    case Direction::LEFT:
        return glm::vec3{-1.0f, 0.0f, 0.0f};
    case Direction::RIGHT:
        return glm::vec3{1.0f, 0.0f, 0.0f};
    case Direction::FRONT:
        return glm::vec3{0.0f, 0.0f, 1.0f};
    case Direction::BACK:
        return glm::vec3{0.0f, 0.0f, -1.0f};
    default:
        return glm::vec3{0.0f};
    }
}
