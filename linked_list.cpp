#include <iostream>
#include <iomanip>
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
      std::min,
      std::setw,
      std::left,
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

        void remove(BookItem *booking) {
            if (booking->prev) booking->prev->next = booking->next;
            else book_head = booking->next;

            if (booking->next) booking->next->prev = booking->prev;
            else book_head = booking->prev;

            delete booking;
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
        int row;
        SeatRow *next;
    };

    struct SeatMap {
        SeatRow *map_head;

        bool linear_search_empty_seat(char seatclass, int &out_row, char &out_col) {
            int min_row, max_row;
            switch (seatclass) {
                case 'F': min_row = 1; max_row = 3; break;
                case 'B': min_row = 4; max_row = 10; break;
                case 'E': min_row = 11; max_row = 30; break;
            }

            SeatRow *curr_row = map_head;
            while (curr_row && curr_row->row < min_row) {
                curr_row = curr_row->next;
            }

            out_row = min_row;
            while (out_row <= max_row) {
                out_col = 'A';
                if (!curr_row || curr_row->row > out_row) return true;

                Seat *curr_seat = curr_row->row_head;
                while (curr_seat && curr_seat->col == out_col) {
                    out_col++;
                    curr_seat = curr_seat->next;
                }

                if (out_col < 'G') return true;

                out_row++;
                curr_row = curr_row->next;
            }

            return false;
        }

        void insert_seat(Seat &seat) {
            SeatRow *prev_row = nullptr;
            SeatRow *curr_row = map_head;
            while (curr_row && curr_row->row < seat.row) {
                prev_row = curr_row;
                curr_row = curr_row->next;
            }

            if (!curr_row || curr_row->row > seat.row) {
                SeatRow *new_row = new SeatRow();
                new_row->row_head = nullptr;
                new_row->next = curr_row;
                new_row->row = seat.row;

                curr_row = new_row;
            }

            if (prev_row) prev_row->next = curr_row;
            else map_head = curr_row;

            Seat *prev_seat = nullptr;
            Seat *curr_seat = curr_row->row_head;
            while (curr_seat && curr_seat->col < seat.col) {
                prev_seat = curr_seat;
                curr_seat = curr_seat->next;
            }

            seat.next = curr_seat;

            if (prev_seat) prev_seat->next = &seat;
            else curr_row->row_head = &seat;
        }

        void remove_seat(Seat &seat) {
            SeatRow *prev_row = nullptr;
            SeatRow *curr_row = map_head;
            while (curr_row && curr_row->row < seat.row) {
                prev_row = curr_row;
                curr_row = curr_row->next;
            }

            if (!curr_row || curr_row->row != seat.row) return;

            Seat *prev_seat = nullptr;
            Seat *curr_seat = curr_row->row_head;
            while (curr_seat && curr_seat->col < seat.col) {
                prev_seat = curr_seat;
                curr_seat = curr_seat->next;
            }

            if (!curr_seat || curr_seat != &seat) return;

            if (prev_seat) prev_seat->next = curr_seat->next;
            else curr_row->row_head = curr_seat->next;

            if (!curr_row->row_head) {
                if (prev_row) prev_row->next = curr_row->next;
                else map_head = curr_row->next;
                delete curr_row;
            }
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

    BookList book_ls { nullptr };
    SeatMap seat_map { nullptr };

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

        auto stop = high_resolution_clock::now();

        csvin.close();

        auto duration = duration_cast<microseconds>(stop - start);
        cout << "[System] Execution Time: " << duration.count() << " µs." << endl
             << "[System] Space Complexity: O(n) | Extra Space: O(1)" << endl
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
        cin.ignore();
        getline(cin, name);

        auto start = high_resolution_clock::now();

        int assign_row;
        char assign_col;
        if (!seat_map.linear_search_empty_seat(seatclass, assign_row, assign_col)) {
            cout << "No available seats in " << class_full_name(seatclass) << " class." << endl;
            return;
        }

        BookItem *booking = new BookItem();
        booking->psg_id = to_string(++book_ls.last_id);
        booking->psg_name = name;
        booking->seat.seatclass = seatclass;
        booking->seat.row = assign_row;
        booking->seat.col = assign_col;

        book_ls.push(booking);
        seat_map.insert_seat(booking->seat);

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        cout << "Reservation Successful! ID: " << booking->psg_id << " | Seat: " << assign_row << assign_col << endl;
        cout << "[System] Execution Time: " << duration.count() << " µs." << endl
             << "[System] Space Complexity (Search Empty Seat): O(Rmin + Rtar*C) | Extra Space: O(1)" << endl
             << "         Space Complexity (Insert Seat): O(R + C) | Extra Space: O(1)" << endl
             << endl;
    }

    void cancel() {
        string target_id;
        cout << "Enter Passenger ID to Cancel: ";
        cin >> target_id;

        auto start = high_resolution_clock::now();

        BookItem *curr_book = book_ls.book_head;
        while (curr_book && curr_book->psg_id != target_id) {
            curr_book = curr_book->next;
        }

        if (!curr_book) {
            cout << "Error: Passenger ID " << target_id << " not found." << endl;
            return;
        }
        seat_map.remove_seat(curr_book->seat);
        cout << "Cancellation confirmed. Seat " << curr_book->seat.row << curr_book->seat.col << " is now available." << endl;
        book_ls.remove(curr_book);

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "[System] Execution Time: " << duration.count() << " µs." << endl
             << "[System] Space Complexity: O(N + R + C) | Extra Space: O(1)";
    }

    void auto_fill() {}

    void lookup() {
        string target_id;
        cout << "Enter Passenger ID: ";
        cin >> target_id;

        auto start = high_resolution_clock::now();
        BookItem *target = book_ls.linear_search_by_id(target_id, nullptr);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        if (target) {
            cout << "-----------------------------" << endl
                 << " PASSENGER FOUND" << endl
                 << "-----------------------------" << endl
                 << " Name  : " << target->psg_name << endl
                 << " Seat  : " << target->seat.row << target->seat.col << endl
                 << " Class : " << class_full_name(target->seat.seatclass) << endl
                 << "-----------------------------" << endl;
        } else {
            cout << "Passenger not found." << endl;
        }

        cout << "[System] Execution Time: " << duration.count() << " µs." << endl
             << "[System] Space Complexity: O(n) | Extra Space: O(1)" << endl
             << endl;
    }

    void print_seat() {
        cout << string(64, '=') << endl
             << string(22, ' ') << "FLIGHT SEATING CHART" << endl
             << string(64, '=') << endl
             << "      A        B        C        D        E        F" << endl;

        auto start = high_resolution_clock::now();

        int next_class = 1;
        int row_count = 1;
        SeatRow *curr_row = seat_map.map_head;
        while (curr_row) {
            if (row_count >= next_class) {
                if (next_class == 1) {
                    cout << endl << "--- [ FIRST CLASS (Rows 1-3) ] ---" << endl;
                    next_class = 4;
                } else if (next_class == 4) {
                    cout << endl << "--- [ BUSINESS CLASS (Rows 4-10) ] ---" << endl;
                    next_class = 11;
                } else if (next_class == 11) {
                    cout << endl << "--- [ ECONOMY CLASS (Rows 11-30) ] ---" << endl;
                    next_class = 31;
                }
            }

            if (row_count < curr_row->row) {
                cout << "..." << endl;
                row_count = min(curr_row->row, next_class);
                continue;
            }

            cout << setw(2) << row_count;

            char col_count = 'A';
            Seat *curr_seat = curr_row->row_head;
            while (col_count < 'G') {
                if (curr_seat && curr_seat->col == col_count) {
                    cout << " [" << curr_seat->passenger->psg_id << "]";
                    curr_seat = curr_seat->next;
                } else {
                    cout << " [------]";
                }
                col_count++;
            }

            cout << endl;
            curr_row = curr_row->next;
            row_count++;
        }

        switch (next_class) {
            case 1:
                cout << endl << "--- [ FIRST CLASS (Rows 1-3) ] ---" << endl << "..." << endl;
            case 4:
                cout << endl << "--- [ BUSINESS CLASS (Rows 4-10) ] ---" << endl << "..." << endl;
            case 11:
                cout << endl << "--- [ ECONOMY CLASS (Rows 11-30) ] ---" << endl << "..." << endl;
        }

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        cout << endl;
        cout << "[System] Execution Time: " << duration.count() << " µs." << endl;
    }

    void print_passenger() {
        BookList psg_ls {nullptr, nullptr};

        auto start = high_resolution_clock::now();

        SeatRow *curr_row = seat_map.map_head;
        while (curr_row) {
            Seat *curr_seat = curr_row->row_head;
            while (curr_seat) {
                psg_ls.push(curr_seat->passenger);
                curr_seat = curr_seat->next;
            }
            curr_row = curr_row->next;
        }

        // TODO: psg_ls.sort_by_id();

        auto stop = high_resolution_clock::now();

        cout << endl << string(64, '=') << endl
             << string(23, ' ') << "PASSENGER MANIFEST" << endl
             << string(64, '=') << endl
             << left
             << setw(8) << "Seat"
             << setw(12) << "ID"
             << setw(25) << "Name"
             << "Class" << endl
             << string(64, '-') << endl;

        BookItem *curr_psg = psg_ls.book_head;
        while (curr_psg) {
            string seat = to_string(curr_psg->seat.row) + curr_psg->seat.col;
            cout << left
                 << setw(8) << seat
                 << setw(12) << curr_psg->psg_id
                 << setw(25) << curr_psg->psg_name
                 << class_full_name(curr_psg->seat.seatclass) << endl;
            curr_psg = curr_psg->next;
        }

        cout << string(64, '=') << endl;

        auto duration = duration_cast<microseconds>(stop - start);
        cout << "[Systen] Execution Time: " << duration.count() << " µs." << endl
        // TODO: count the Space Complexity;
             << "[System] Space Complexity: O(?) | Extra Space: O(?)" << endl << endl;
    }

    void dispatch() {
        auto start = high_resolution_clock::now();

        SeatRow *curr_row = seat_map.map_head;
        while (curr_row) {
            Seat *curr_seat = curr_row->row_head;
            while (curr_seat) {
                Seat *next_seat = curr_seat->next;
                BookItem *psg = curr_seat->passenger;
                book_ls.remove(psg);
                curr_seat = next_seat;
            }

            SeatRow *next_row = curr_row->next;
            delete curr_row;
            curr_row = next_row;
        }
        seat_map.map_head = nullptr;

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        cout << "Dispatch Complete. Session cleared and records marked." << endl;
        cout << "[System] Execution Time: " << duration.count() << " µs." << endl;
    }

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
    }
}
