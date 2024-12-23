#include <fstream>
#include <iosfwd>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include "axolote/gl/texture.hpp"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "minecraft/atlas_mapping_uvs.hpp"
#include "minecraft/utils.hpp"

std::shared_ptr<axolote::gl::Texture> atlas_texture = nullptr;

const int type_to_index(std::string type) {
    if (type == "air") {
        return 0;
    }
    else if (type == "grass") {
        return 1;
    }
    else {
        return -1;
    }
}

const int subtype_to_index(std::string subtype) {
    if (subtype == "top") {
        return 0;
    }
    else if (subtype == "bottom") {
        return 1;
    }
    else if (subtype == "left") {
        return 2;
    }
    else if (subtype == "right") {
        return 3;
    }
    else if (subtype == "front") {
        return 4;
    }
    else if (subtype == "back") {
        return 5;
    }
    else {
        return -1;
    }
}

std::unordered_map<int, glm::vec4> read_from_file() {
    std::cout << "Processing atlas mapping" << std::endl;
    std::unordered_map<int, glm::vec4> atlas_mapping_uvs;
    std::ifstream file(my_get_path("./resources/textures/atlas1.txt"));
    std::string line;
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

        std::size_t last_underline_index = name.find_last_of('_');
        std::string type = name.substr(0, last_underline_index);
        std::string subtype = name.substr(last_underline_index + 1);
        int type_index = type_to_index(type);
        int subtype_index = subtype_to_index(subtype);
        if (type_index == -1 || subtype_index == -1) {
            std::cerr << "Invalid type or subtype" << std::endl;
            continue;
        }

        atlas_mapping_uvs[type_index + subtype_index] = glm::vec4{x, y, w, h};
        std::cout << glm::to_string(
            atlas_mapping_uvs[type_index + subtype_index]
        ) << std::endl;
    }
    return atlas_mapping_uvs;
}

const std::unordered_map<int, glm::vec4> &get_atlas_mapping_uvs() {
    static std::unordered_map<int, glm::vec4> atlas_mapping_uvs
        = read_from_file();
    return atlas_mapping_uvs;
}
