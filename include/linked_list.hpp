#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <string>

namespace fms::llist {
    struct BookItem {
        std::string psg_id;
        std::string psg_name;
        int seat_row;
        char seat_col;
        char seat_class;
        BookItem *prev;
        BookItem *next;
    };

    extern BookItem *book_head;
    extern BookItem *book_tail;

    void push_book(BookItem *booking);

    void setup();

    void reserve();
    void cancel();
    void lookup();
    void print_seat();
    void print_passenger();

    void teardown();
}
#endif
