#pragma once

#include <string>
#include <unordered_map>

#include "axolote/gl/texture.hpp"
#include <glm/glm.hpp>

extern std::unordered_map<std::string, glm::vec4> atlas_mapping_uvs;
extern std::shared_ptr<axolote::gl::Texture> atlas_texture;

std::unordered_map<std::string, glm::vec4> read_from_file();
