#pragma once

#include <string>

#include "axolote/gl/ebo.hpp"
#include "axolote/gl/vao.hpp"
#include "axolote/gl/vbo.hpp"
#include "axolote/gl/texture.hpp"

class Face {
public:
    enum class Direction { TOP, BOTTOM, LEFT, RIGHT, FRONT, BACK };

    // VAO
    static std::shared_ptr<axolote::gl::VAO> vao;
    // VBO
    static std::shared_ptr<axolote::gl::VBO> vbo;
    // EBO
    static std::shared_ptr<axolote::gl::EBO> ebo;
    // Texture
    static std::shared_ptr<axolote::gl::Texture> texture;

    static void init();

    glm::vec3 normal{0.0f};
    glm::vec4 uv{0.0f};

    Face();
    Face(std::string type);

    Direction get_direction_from_string(std::string str);
    glm::vec3 get_normal_from_direction(Direction direction);

private:
    static const std::vector<GLuint> indices;
};
