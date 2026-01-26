// #include <iostream>
#include "include/menu.hpp"

int main() {
    void (*menu_ps)(void *);
    void (*next_m)(void *) = fms::menu::main_m;

    while (next_m != nullptr) {
        menu_ps = next_m;
        next_m = nullptr;
        menu_ps(&next_m);
    }
}
