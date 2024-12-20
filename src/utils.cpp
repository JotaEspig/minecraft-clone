#include <string>

#include "minecraft/utils.hpp"

std::string my_get_path(const std::string &path) {
    return std::string(PROJECT_ROOT_FOLDER) + "/" + path;
}
