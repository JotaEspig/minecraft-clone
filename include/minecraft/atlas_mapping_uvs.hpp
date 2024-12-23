#pragma once

#include <string>
#include <unordered_map>

#include "axolote/gl/texture.hpp"
#include <glm/glm.hpp>

enum class BlockType { AIR = 0, GRASS };

extern std::shared_ptr<axolote::gl::Texture> atlas_texture;

const std::unordered_map<int, glm::vec4>& get_atlas_mapping_uvs();
