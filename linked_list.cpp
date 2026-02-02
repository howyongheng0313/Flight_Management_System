#include <iostream>
#include <fstream>
#include <sstream>
#include "include/linked_list.hpp"

using std::cout,
      std::endl,
      std::getline,
      std::string,
      std::ifstream,
      std::stringstream;

namespace fms::llist {
    BookItem *book_head = nullptr;
    BookItem *book_tail = nullptr;

    void push_book(BookItem *booking) {
        booking->prev = book_tail;
        booking->next = nullptr;

        if (book_head == nullptr) {
            book_head = booking;
            book_tail = booking;
            return;
        }

        book_tail->next = booking;
        book_tail = booking;
    }

    void setup() {
        string row;
        ifstream csvin("./dataset/flight_passenger_data.csv");

        getline(csvin, row);
        while (getline(csvin, row)) {
            stringstream row_stream(row);
            BookItem *booking = new BookItem();
            string seat_row;
            string seat_col;
            string seat_class;

            getline(row_stream, booking->psg_id, ',');
            getline(row_stream, booking->psg_name, ',');
            getline(row_stream, seat_row, ',');
            getline(row_stream, seat_col, ',');
            getline(row_stream, seat_class, ',');

            booking->seat_row = std::stoi(seat_row);
            booking->seat_col = seat_col[0];
            booking->seat_class = seat_class[0];

            push_book(booking);
        }
        csvin.close();
    }

    BookItem *linear_search_by_id(const string& targetId, BookItem *entry = book_head) {
        BookItem *curr = entry;
        while (curr) {
            if (curr->psg_id == targetId)
                return curr;
            curr = curr->next;
        }
        return nullptr;
    }

    BookItem *linear_search_by_seat(int row, char col, BookItem *entry = book_head) {
        BookItem *curr = entry;
        while (curr) {
            if (curr->seat_row == row && curr->seat_col == col)
                return curr;
            curr = curr->next;
        }
        return nullptr;
    }

    void reserve() {}

    void cancel() {}

    void lookup() {}

    void print_seat() {}

    void print_passenger() {}

    void teardown() {
        BookItem *curr = book_head;
        while (curr != nullptr) {
            BookItem *next = curr->next;
            delete curr;
            curr = next;
        }
        book_head = nullptr;
        book_tail = nullptr;
    }
}
