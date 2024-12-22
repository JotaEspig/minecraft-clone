#include <memory>
#include <string>
#include <vector>

#include "axolote/gl/ebo.hpp"
#include "axolote/gl/vao.hpp"
#include "axolote/gl/vbo.hpp"

#include "minecraft/atlas_mapping_uvs.hpp"
#include "minecraft/face.hpp"
#include "minecraft/utils.hpp"

// Pos, Color (vec4), Normal, TexCoord
std::vector<glm::vec3> default_vertices = {
    // clang-format off
    {-1.0f, -1.0f, 0.0f},
    {1.0f, -1.0f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {-1.0f, 1.0f, 0.0f},
    // clang-format on
};

std::shared_ptr<axolote::gl::VAO> Face::vao = nullptr;
std::shared_ptr<axolote::gl::VBO> Face::vbo = nullptr;
std::shared_ptr<axolote::gl::EBO> Face::ebo = nullptr;
std::shared_ptr<axolote::gl::Texture> Face::texture = nullptr;
const std::vector<GLuint> Face::indices = {
    0, 1, 2, 2, 3, 0,
};

void Face::init() {
    if (vao) {
        return;
    }
    texture = axolote::gl::Texture::create(
        my_get_path("./resources/textures/atlas1.png"), "diffuse", (GLuint)0
    );

    vao = axolote::gl::VAO::create();
    vao->bind();
    vbo = axolote::gl::VBO::create(default_vertices);
    vbo->bind();
    ebo = axolote::gl::EBO::create(indices);
    ebo->bind();

    vbo->bind();
    vao->link_attrib(vbo, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void *)0);

    vao->unbind();
    vbo->unbind();
}

Face::Face() {
    init();
}

Face::Face(std::string type) {
    init();

    glm::vec4 uv_for_type = atlas_mapping_uvs.at(type);
    Direction direction = get_direction_from_string(type);
    normal = get_normal_from_direction(direction);

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

Face::Direction Face::get_direction_from_string(std::string str) {
    std::string last_part = str.substr(str.find_last_of("_") + 1);
    if (last_part == "top") {
        return Direction::TOP;
    }
    else if (last_part == "bottom") {
        return Direction::BOTTOM;
    }
    else if (last_part == "left") {
        return Direction::LEFT;
    }
    else if (last_part == "right") {
        return Direction::RIGHT;
    }
    else if (last_part == "front") {
        return Direction::FRONT;
    }
    else if (last_part == "back") {
        return Direction::BACK;
    }
    return Direction::TOP;
}

glm::vec3 Face::get_normal_from_direction(Direction direction) {
    switch (direction) {
    case Direction::TOP:
        return {0.0f, 1.0f, 0.0f};
    case Direction::BOTTOM:
        return {0.0f, -1.0f, 0.0f};
    case Direction::LEFT:
        return {-1.0f, 0.0f, 0.0f};
    case Direction::RIGHT:
        return {1.0f, 0.0f, 0.0f};
    case Direction::FRONT:
        return {0.0f, 0.0f, 1.0f};
    case Direction::BACK:
        return {0.0f, 0.0f, -1.0f};
    }
    return {0.0f, 0.0f, 0.0f};
}
