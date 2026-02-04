// #ifndef ARRAY_BASED_HPP
// #define ARRAY_BASED_HPP

// #include <string>

// namespace fms::array {
//     constexpr int MAX_BOOKING = 16384;

//     struct BookItem {
//         std::string psg_id;
//         std::string psg_name;
//         int seat_row;
//         char seat_col;
//         char seat_class;
//     };

//     void setup();

//     void reserve();
//     void cancel();
//     void auto_fill();
//     void lookup();
//     void print_seat();
//     void print_passenger();
//     void dispatch();

//     void teardown();
// }
// #endif

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
        
        bool is_assigned;
        bool is_new_reservation; // Track new vs historical data
        bool was_dispatched;
    };

    extern BookItem book_ls[MAX_BOOKING];
    extern int seat_map[31][6];
    extern int bookingCount;
    extern bool is_sync_confirmed; // The control flag

    void setup();
    void auto_fill();
    void reserve();
    void cancel();
    void lookup();
    void print_seat();
    void print_passenger();
    void dispatch();

    int linearSearchByID(const std::string& targetID);
    void bubbleSortByID(BookItem arr[], int n);
}
#endif
