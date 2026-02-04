#ifndef MENU_HPP
#define MENU_HPP

#include <string>

namespace fms::menu {
    void main_m(void *next_m);
    void array_m(void *next_m);
    void llist_m(void *next_m);
    char base_m(std::string title);
}
#endif
