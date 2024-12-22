# Author: João Vitor Espig

header = "#pragma once\n\n#include <glm/glm.hpp>\n\n// clang-format off\n\n"

with open("atlas1.txt", "r") as file:
    lines = file.readlines()
    for line in lines:
        name, x, y, w, h = line.split()
        header += f"#define {name.upper()}_UV glm::vec4{{ {x}f, {y}f, {w}f, {h}f }};\n"

with open("atlas1.hpp", "w") as file:
    file.write(header)
