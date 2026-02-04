#ifndef ARRAY_BASED_HPP
#define ARRAY_BASED_HPP

#include <string>

namespace fms::array {
    constexpr int MAX_BOOKING = 16384;

    struct BookItem {
        std::string psg_id;
        std::string psg_name;
        int seat_row;
        char seat_col;
        char seat_class;
    };

    void setup();

    void reserve();
    void cancel();
    void auto_fill();
    void lookup();
    void print_seat();
    void print_passenger();
    void dispatch();

    void teardown();
}
#endif
