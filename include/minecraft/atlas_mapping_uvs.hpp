#pragma once

#include <string>
#include <unordered_map>

#include "axolote/gl/texture.hpp"
#include <glm/glm.hpp>

extern std::shared_ptr<axolote::gl::Texture> atlas_texture;

const std::unordered_map<std::string, glm::vec4>& get_atlas_mapping_uvs();
