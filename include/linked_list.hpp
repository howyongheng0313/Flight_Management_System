#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

namespace fms::llist {
    struct BookItem;
    struct BookList;
    struct Seat;
    struct SeatRow;
    struct SeatMap;

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
