#include <iostream>

#include "minecraft/app.hpp"

int main() {
    std::cout << "Axolote Engine" << std::endl;
    App app{};
    app.set_title("Window");
    app.set_maximized();
    app.set_color(0x01, 0x40, 0xff);
    app.main_loop();
    return 0;
}
