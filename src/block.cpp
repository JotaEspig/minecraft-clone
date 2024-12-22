#include "minecraft/block.hpp"

Block::Block() :
  position{glm::vec3{0.0f, 0.0f, 0.0f}},
  type{"air"} {
}

Block::Block(glm::vec3 position, std::string type) :
  position{position},
  type{type} {
    faces[0] = Face{type + "_top"};
    faces[1] = Face{type + "_bottom"};
    faces[2] = Face{type + "_left"};
    faces[3] = Face{type + "_right"};
    faces[4] = Face{type + "_front"};
    faces[5] = Face{type + "_back"};
}
