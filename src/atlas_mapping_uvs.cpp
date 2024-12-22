#include "minecraft/utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "minecraft/atlas_mapping_uvs.hpp"

std::unordered_map<std::string, glm::vec4> read_from_file() {
    std::unordered_map<std::string, glm::vec4> atlas_mapping_uvs;
    std::ifstream file(my_get_path("./resources/textures/atlas1.txt"));
    std::string line;
    // check to see if the file is open
    if (!file.is_open()) {
        std::cerr << "Could not open file" << std::endl;
        return atlas_mapping_uvs;
    }

    while (std::getline(file, line)) {
        std::cout << line << std::endl;
        std::string name;
        float x, y, w, h;
        std::stringstream ss{line};
        ss >> name >> x >> y >> w >> h;
        atlas_mapping_uvs[name] = glm::vec4{x, y, w, h};
        std::cout << glm::to_string(atlas_mapping_uvs[name]) << std::endl;
    }
    return atlas_mapping_uvs;
}

std::unordered_map<std::string, glm::vec4> atlas_mapping_uvs;
