#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "include/linked_list.hpp"

using std::cout,
      std::cin,
      std::endl,
      std::getline,
      std::stoi,
      std::to_string,
      std::string,
      std::ifstream,
      std::stringstream;
using std::chrono::high_resolution_clock,
      std::chrono::microseconds,
      std::chrono::duration_cast;

namespace fms::llist {
    struct Seat {
        BookItem *passenger;
        int row;
        char col;
        char seatclass;
        Seat *next;
    };

    struct BookItem {
        string psg_id;
        string psg_name;
        Seat seat;
        BookItem *prev;
        BookItem *next;

        BookItem() {
            seat.passenger = this;
        }
    };

    struct BookList {
        BookItem *book_head;
        BookItem *book_tail;
        int last_id = 0;

        void push(BookItem *booking) {
            booking->prev = book_head;
            booking->next = nullptr;

            if (!book_head) {
                book_head = booking;
                book_tail = booking;
                return;
            }

            book_tail->next = booking;
            book_tail = booking;
        }

        void insert(BookItem *lead, BookItem *booking) {
            booking->prev = lead;

            if (!lead) {
                book_head = booking;
                book_tail = booking;
                booking->next = nullptr;
                return;
            }

            if (!lead->next) {
                book_tail = booking;
            }

            booking->next = lead->next;
            lead->next = booking;
        }

        BookItem *linear_search_by_id(const string& target_id, BookItem *entry) {
            BookItem *curr = entry ? entry : book_head;
            while (curr) {
                if (curr->psg_id == target_id)
                    return curr;
                curr = curr->next;
            }
            return nullptr;
        }

        BookItem *linear_search_by_seat(int row, char col, BookItem *entry) {
            BookItem *curr = entry ? entry : book_head;
            while (curr) {
                if (curr->seat.row == row && curr->seat.col == col)
                    return curr;
                curr = curr->next;
            }
            return nullptr;
        }
    };

    struct SeatRow {
        Seat *row_head;
        Seat *row_tail;
        int row;
        SeatRow *next;
    };

    struct SeatMap {
        SeatRow *map_head;
        SeatRow *first_head;
        SeatRow *business_head;
        SeatRow *economy_head;

        bool linear_search_empty_seat(char seatclass, int &out_row, char &out_col) {
            SeatRow *class_head;
            int min_row, max_row;
            switch (seatclass) {
                case 'F':
                    class_head = first_head;
                    min_row = 1;
                    max_row = 3;
                    break;
                case 'B':
                    class_head = business_head;
                    min_row = 4;
                    max_row = 10;
                    break;
                case 'E':
                    class_head = economy_head;
                    min_row = 11;
                    max_row = 30;
                    break;
            }

            if (!class_head) {
                out_row = min_row;
                out_col = 'A';
                return true;
            }

            SeatRow *curr_row = class_head;
            while (curr_row && curr_row->row <= max_row) {
                Seat *curr_seat = curr_row->row_head;

                out_col = 'A';
                while (curr_seat && curr_seat->col == out_col) {
                    out_col++;
                    curr_seat = curr_seat->next;
                }
                if (out_col < 'G') {
                    out_row = curr_row->row;
                    return true;
                }

                curr_row = curr_row->next;
            }

            return false;
        }

        void insert_seat(Seat &seat) {
            // TODO: insert a seat, if the row is not existing, create a new row;
            SeatRow *class_head;
            switch (seat.seatclass) {
                case 'F': class_head = first_head; break;
                case 'B': class_head = business_head; break;
                case 'E': class_head = economy_head; break;
            }

            if (!map_head) {
                map_head = new SeatRow();
                map_head->row_head = nullptr;
                map_head->row_tail = nullptr;
                map_head->next = nullptr;
                map_head->row = seat.row;
            }

            if (!class_head) {
                class_head = map_head;
            }

            SeatRow *curr_row = class_head;
            while (curr_row->next && curr_row->next->row <= seat.row) {
                curr_row = curr_row->next;
            }

            if (curr_row->row < seat.row) {
                SeatRow *new_row = new SeatRow();
                new_row->row_head = nullptr;
                new_row->row_tail = nullptr;
                new_row->next = curr_row->next;
                new_row->row = seat.row;

                curr_row->next = new_row;
                curr_row = new_row;
            }

            Seat *curr_seat = curr_row->row_head;
            if (!curr_seat) {}
        }
    };

    string class_full_name(char seatclass) {
        switch (seatclass) {
            case 'F': return "First";
            case 'B': return "Business";
            case 'E': return "Economy";
            default: return "Unknown";
        }
    }

    BookList book_ls { nullptr, nullptr };
    SeatMap seat_map { nullptr, nullptr, nullptr, nullptr };

    void setup() {
        string row;
        ifstream csvin("./dataset/flight_passenger_data.csv");

        auto start = high_resolution_clock::now();

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

            booking->seat.row = stoi(seat_row);
            booking->seat.col = seat_col[0];
            booking->seat.seatclass = seat_class[0];

            book_ls.push(booking);

            int num_id = stoi(booking->psg_id);
            if (num_id > book_ls.last_id) {
                book_ls.last_id = num_id;
            }
        }

        auto end = high_resolution_clock::now();

        csvin.close();

        auto duration = duration_cast<microseconds>(end - start);
        cout << "[System] Execution Time: " << duration.count() << "µs." << endl
             << "[System] Space Complexity: O(n) | Extra Space: O(n)" << endl
             << endl;
    }

    void reserve() {
        char seatclass;
        cout << "Select class (F = First / B = Business / E = Economy): ";
        cin >> seatclass;
        seatclass = toupper(seatclass);

        if (seatclass != 'F' && seatclass != 'B' && seatclass != 'E') {
            cout << "Invalid Class." << endl;
            return;
        }

        string name;
        cout << "Enter Passenger Name: ";
        getline(cin, name);

        int assign_row;
        char assign_col;
        if (!seat_map.linear_search_empty_seat(seatclass, assign_row, assign_col)) {
            cout << "No available seats in " << class_full_name(seatclass) << " class." << endl;
            return;
        }

        BookItem *booking = new BookItem();
        booking->psg_id = to_string(book_ls.last_id++);
        booking->psg_name = name;
        booking->seat.seatclass = seatclass;
        booking->seat.row = assign_row;
        booking->seat.col = assign_col;

        book_ls.push(booking);
        seat_map.insert_seat(booking->seat);  // TODO: fix the function
    }

    void cancel() {}

    void auto_fill() {}

    void lookup() {}

    void print_seat() {}

    void print_passenger() {}

    void dispatch() {}

    void teardown() {
        BookItem *curr_book = book_ls.book_head;
        while (curr_book) {
            BookItem *next = curr_book->next;
            delete curr_book;
            curr_book = next;
        }
        book_ls.book_head = nullptr;
        book_ls.book_tail = nullptr;

        SeatRow *curr_row = seat_map.map_head;
        while (curr_row) {
            SeatRow *next = curr_row->next;
            delete curr_row;
            curr_row = next;
        }
        seat_map.map_head = nullptr;
        seat_map.first_head = nullptr;
        seat_map.business_head = nullptr;
        seat_map.economy_head = nullptr;
    }
}
