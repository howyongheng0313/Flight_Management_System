#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <limits>
#include "include/linked_list.hpp"

using std::cout;
using std::cin;
using std::endl;
using std::getline;
using std::stoi;
using std::to_string;
using std::string;
using std::ifstream;
using std::stringstream;

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;
using std::chrono::duration_cast;

namespace fms::llist {

    struct Seat {
        int row;
        char col;
        char seatclass;
        struct BookItem *passenger;
        Seat *next;
    };

    struct BookItem {
        string psg_id;
        string psg_name;
        Seat seat;
        bool is_assigned;
        bool is_new_reservation;
        bool was_dispatched;
        BookItem *prev;
        BookItem *next;

        BookItem() {
            seat.passenger = this;
            seat.next = nullptr;
        }
    };

    struct BookList {
        BookItem *book_head = nullptr;
        BookItem *book_tail = nullptr;
        int last_id = 100000;

        void push(BookItem *node) {
            node->prev = book_tail;
            node->next = nullptr;
            if (!book_head) {
                book_head = book_tail = node;
                return;
            }
            book_tail->next = node;
            book_tail = node;
        }

        BookItem *linear_search_by_id(const string &id) {
            BookItem *curr = book_head;
            while (curr) {
                if (curr->psg_id == id) return curr;
                curr = curr->next;
            }
            return nullptr;
        }
    };

    struct SeatRow {
        Seat *row_head;
        SeatRow *next;
        int row;
    };

    struct SeatMap {
        SeatRow *map_head = nullptr;

        bool linear_search_empty_seat(char cls, int &out_row, char &out_col) {
            int min_row = (cls == 'F' ? 1 : (cls == 'B' ? 4 : 11));
            int max_row = (cls == 'F' ? 3 : (cls == 'B' ? 10 : 30));
            SeatRow *curr_row = map_head;

            for (int r = min_row; r <= max_row; r++) {
                SeatRow *row_ptr = curr_row;
                while (row_ptr && row_ptr->row < r) row_ptr = row_ptr->next;

                char col = 'A';
                Seat *seat_ptr = (row_ptr && row_ptr->row == r) ? row_ptr->row_head : nullptr;

                while (seat_ptr && seat_ptr->col == col) {
                    col++;
                    seat_ptr = seat_ptr->next;
                }

                if (col <= 'F') {
                    out_row = r;
                    out_col = col;
                    return true;
                }
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

            if (!curr_row || curr_row->row != seat.row) {
                SeatRow *new_row = new SeatRow{nullptr, curr_row, seat.row};
                if (prev_row) prev_row->next = new_row;
                else map_head = new_row;
                curr_row = new_row;
            }

            Seat *prev = nullptr;
            Seat *curr = curr_row->row_head;
            while (curr && curr->col < seat.col) {
                prev = curr;
                curr = curr->next;
            }

            seat.next = curr;
            if (prev) prev->next = &seat;
            else curr_row->row_head = &seat;
        }

        void clear() {
            SeatRow *curr = map_head;
            while (curr) {
                SeatRow *next = curr->next;
                delete curr;
                curr = next;
            }
            map_head = nullptr;
        }
    };

    static BookList book_ls;
    static SeatMap seat_map;

    // =====================================================
    // 1. HELPER FUNCTIONS
    // =====================================================

    string class_full_name(char c) {
        switch (c) {
            case 'F': return "First";
            case 'B': return "Business";
            case 'E': return "Economy";
            default: return "Unknown";
        }
    }

    // =====================================================
    // 2. MAIN FUNCTIONS
    // =====================================================

    void setup() {
        teardown();
        auto start = high_resolution_clock::now();
        ifstream csv("./dataset/flight_passenger_data.csv");

        if (!csv.is_open()) {
            cout << "Error opening dataset." << endl;
            return;
        }

        string line;
        getline(csv, line);

        while (getline(csv, line)) {
            stringstream ss(line);
            string id, name, srow, scol, scls;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, srow, ',');
            getline(ss, scol, ',');
            getline(ss, scls, ',');

            BookItem *node = new BookItem();
            node->psg_id = id;
            node->psg_name = name;
            node->seat.row = stoi(srow);
            node->seat.col = scol[0];
            node->seat.seatclass = scls[0];
            node->is_assigned = false;
            node->is_new_reservation = false;
            node->was_dispatched = false;

            book_ls.push(node);

            int num = stoi(id);
            if (num > book_ls.last_id) book_ls.last_id = num;
        }

        csv.close();
        auto stop = high_resolution_clock::now();
        cout << "[System] Setup Complete. Loaded records." << endl;
        cout << "[System] Setup Execution Time: "
             << duration_cast<microseconds>(stop - start).count() << " us" << endl << endl;
    }

    void reserve() {
        char sc;
        do {
            cout << "Select Class (F/B/E): ";
            cin >> sc;
            sc = std::toupper(sc);
        } while (sc != 'F' && sc != 'B' && sc != 'E');

        cin.ignore();
        cout << "Enter Passenger Name: ";
        string name; getline(cin, name);

        int r; char c;
        if (!seat_map.linear_search_empty_seat(sc, r, c)) {
            cout << "No seats available in " << class_full_name(sc) << " class." << endl;
            return;
        }

        BookItem *node = new BookItem();
        node->psg_id = to_string(++book_ls.last_id);
        node->psg_name = name;
        node->seat.row = r;
        node->seat.col = c;
        node->seat.seatclass = sc;
        node->is_assigned = true;
        node->is_new_reservation = true;
        node->was_dispatched = false;

        book_ls.push(node);
        seat_map.insert_seat(node->seat);

        cout << "Reservation Successful! ID: " << node->psg_id
             << " | Seat: " << r << c << endl << endl;
    }

    void cancel() {}

    void auto_fill() {}

    void lookup() {
        string id;
        cout << "Enter Passenger ID: ";
        cin >> id;

        auto start = high_resolution_clock::now();
        BookItem *node = book_ls.linear_search_by_id(id);
        auto stop = high_resolution_clock::now();

        if (!node) {
            cout << "Passenger not found." << endl;
            return;
        }

        cout << "\n-----------------------------\n";
        cout << " PASSENGER FOUND\n";
        cout << "-----------------------------\n";
        cout << " Name  : " << node->psg_name << endl;
        cout << " Seat  : " << node->seat.row << node->seat.col << endl;
        cout << " Class : " << class_full_name(node->seat.seatclass) << endl;
        cout << "-----------------------------\n";
        cout << "Lookup Execution Time: "
             << duration_cast<microseconds>(stop - start).count() << " us\n\n";
    }

    void print_seat() {
        cout << "\n" << string(64, '=') << "\n";
        cout << "\t\t   FLIGHT SEATING CHART\n";
        cout << string(64, '=') << "\n";
        cout << "      A       B       C       D       E       F\n";

        for (int r = 1; r <= 30; r++) {
            cout << std::setw(2) << r << " ";
            for (char c = 'A'; c <= 'F'; c++) {
                BookItem *found = book_ls.book_head;
                string id = "------";
                while (found) {
                    if (found->seat.row == r && found->seat.col == c && found->is_assigned) {
                        id = found->psg_id;
                        break;
                    }
                    found = found->next;
                }
                cout << "[" << std::setw(6) << id << "]";
            }
            cout << endl;
        }
        cout << string(64, '=') << "\n\n";
    }

void print_passenger() {
        cout << "Do you want to sort by:\n";
        cout << "1. Passenger ID\n";
        cout << "2. Seat\n";
        cout << "3. Passenger's Name\n";
        cout << "Select option: ";

        int criteria;
        if (!(cin >> criteria)) {
            cin.clear(); 
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            criteria = 1;
        }

        if (criteria < 1 || criteria > 3) {
            cout << "Invalid option. Defaulting to Passenger ID.\n";
            criteria = 1;
        }

        struct TempNode {
            string id;
            string name;
            int row;
            char col;
            char seatclass;
            TempNode *next;
        };

        TempNode *head = nullptr;
        TempNode *tail = nullptr;
        BookItem *curr = book_ls.book_head;


        while (curr) {
            if (curr->is_assigned) {
                TempNode *newNode = new TempNode{
                    curr->psg_id,
                    curr->psg_name,
                    curr->seat.row,
                    curr->seat.col,
                    curr->seat.seatclass,
                    nullptr
                };

                if (!head) {
                    head = newNode;
                    tail = newNode;
                } else {
                    tail->next = newNode;
                    tail = newNode;
                }
            }
            curr = curr->next;
        }

        if (!head) {
            cout << "[System] No passengers found in manifest.\n";
            return;
        }

        bool swapped;
        TempNode *ptr1;
        TempNode *lptr = nullptr; 

        do {
            swapped = false;
            ptr1 = head;

            while (ptr1->next != lptr) {
                bool needSwap = false;

                if (criteria == 1) {
                    try {
                        if (std::stoll(ptr1->id) > std::stoll(ptr1->next->id)) {
                            needSwap = true;
                        }
                    } catch (...) {

                        if (ptr1->id > ptr1->next->id) needSwap = true;
                    }
                } 
                else if (criteria == 2) {
                    if (ptr1->row > ptr1->next->row) {
                        needSwap = true;
                    } else if (ptr1->row == ptr1->next->row) {
                        if (ptr1->col > ptr1->next->col) {
                            needSwap = true;
                        }
                    }
                } 
                else if (criteria == 3) {
                    if (ptr1->name > ptr1->next->name) {
                        needSwap = true;
                    }
                }

                if (needSwap) {
                    std::swap(ptr1->id, ptr1->next->id);
                    std::swap(ptr1->name, ptr1->next->name);
                    std::swap(ptr1->row, ptr1->next->row);
                    std::swap(ptr1->col, ptr1->next->col);
                    std::swap(ptr1->seatclass, ptr1->next->seatclass);
                    swapped = true;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1; 
        } while (swapped);

        cout << "\n" << string(64, '=') << "\n";
        cout << "                   PASSENGER MANIFEST\n";
        cout << string(64, '=') << "\n";
        cout << std::left 
             << std::setw(8) << "Seat" 
             << std::setw(12) << "ID" 
             << std::setw(25) << "Name" 
             << "Class\n";
        cout << string(64, '-') << "\n";

        TempNode *t = head;
        while (t) {
            string fullClass;
            switch(t->seatclass) {
                case 'F': fullClass = "First"; break;
                case 'B': fullClass = "Business"; break;
                case 'E': fullClass = "Economy"; break;
                default: fullClass = "Unknown";
            }

            string seatStr = to_string(t->row) + t->col;

            cout << std::left
                 << std::setw(8) << seatStr
                 << std::setw(12) << t->id
                 << std::setw(25) << t->name
                 << fullClass << "\n";

            TempNode *toDelete = t;
            t = t->next;
            delete toDelete;
        }

        cout << string(64, '=') << "\n\n";
    }

    void dispatch() {
        BookItem *curr = book_ls.book_head;

        while (curr) {
            BookItem *next = curr->next;
            delete curr; 
            curr = next;
        }

        book_ls.book_head = nullptr;
        book_ls.book_tail = nullptr;
        seat_map.clear();
        cout << "Dispatch Complete. All passengers removed and session cleared." << endl << endl;
    }

    void teardown() {
        BookItem *curr = book_ls.book_head;
        while (curr) {
            BookItem *next = curr->next;
            delete curr;
            curr = next;
        }
        book_ls.book_head = nullptr;
        book_ls.book_tail = nullptr;
        seat_map.clear();
    }
}