#include "minecraft/block.hpp"

Block::Block() :
  type{BlockType::AIR} {
    faces_directions.fill(Face::Direction::INVALID);
}

Block::Block(BlockType type) : type{type} {
    faces_directions[0] = Face::Direction::TOP;
    faces_directions[1] = Face::Direction::BOTTOM;
    faces_directions[2] = Face::Direction::LEFT;
    faces_directions[3] = Face::Direction::RIGHT;
    faces_directions[4] = Face::Direction::FRONT;
    faces_directions[5] = Face::Direction::BACK;
}
