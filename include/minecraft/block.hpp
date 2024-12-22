#pragma once

#include "minecraft/face.hpp"
#include <string>

#include <glm/glm.hpp>

class Block {
public:
    glm::vec3 position;
    std::string type;

    std::vector<Face> faces{6};

    Block();
    Block(glm::vec3 position, std::string type);
};
