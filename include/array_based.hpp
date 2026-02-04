#ifndef ARRAY_BASED_HPP
#define ARRAY_BASED_HPP

#include <string>

namespace fms::array {
    constexpr int MAX_BOOKING = 16384;
    constexpr int MAX_ROWS = 30;
    constexpr int MAX_COLS = 6;

    struct BookItem;

    void setup();
    void auto_fill();
    void reserve();
    void cancel();
    void auto_fill();
    void lookup();
    void print_seat();
    void print_passenger();
    void dispatch();

    int linearSearchByID(const std::string& targetID);
    void bubbleSortByID(BookItem arr[], int n);
}
#endif
