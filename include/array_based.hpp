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

    extern BookItem book_ls[MAX_BOOKING];

    void setup();

    void reserve();
    void cancel();
    void lookup();
    void print_seat();
    void print_passenger();

    void teardown();
}
#endif
