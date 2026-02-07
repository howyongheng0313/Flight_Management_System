#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <chrono>
#include "include/array_based.hpp"

using std::cout,
      std::cin,
      std::left,
      std::setw,
      std::endl,
      std::getline,
      std::to_string,
      std::ios,
      std::string,
      std::ifstream,
      std::ofstream,
      std::stringstream,
      std::numeric_limits,
      std::streamsize;
using std::chrono::high_resolution_clock,
      std::chrono::microseconds,
      std::chrono::duration_cast;

static const string CSV_FILE = "./dataset/flight_passenger_data.csv";

namespace fms::array {
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

    BookItem book_ls[MAX_BOOKING];
    int seat_map[31][6];  // TODO: Fix wrong row range
    int bookingCount = 0;

    static const string CSV_FILE = "./dataset/flight_passenger_data.csv";

    // ==========================================================
    // 1. ISOLATED ALGORITHMS (Technical Requirements)
    // ==========================================================

    int linearSearchByID(const string& targetID) {
        for (int i = 0; i < bookingCount; i++) {
            if (book_ls[i].psg_id == targetID) return i;
        }
        return -1;
    }

    bool linearSearchEmptySeat(int rowMin, int rowMax, int& outRow, int& outColIdx) {
        for (int r = rowMin; r <= rowMax; r++) {
            for (int c = 0; c < 6; c++) {
                if (seat_map[r][c] == -1) {
                    outRow = r;
                    outColIdx = c;
                    return true;
                }
            }
        }
        return false;
    }

    void bubbleSortByID(BookItem arr[], int n) {
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (stoi(arr[j].psg_id) > stoi(arr[j + 1].psg_id)) {
                    BookItem t = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = t;
                }
            }
        }
    }

    int getMaxPassengerID() {
        int m = 100000;
        for (int i = 0; i < bookingCount; i++) {
            try {
                int c = stoi(book_ls[i].psg_id);
                if (c > m) m = c;
            } catch (...) {}
        }
        return m;
    }

    void rebuildSeatMap() {
        for (int r = 0; r <= 30; r++)
            for (int c = 0; c < 6; c++) seat_map[r][c] = -1;

        for (int i = 0; i < bookingCount; i++) {
            if (book_ls[i].is_assigned) {
                int r = book_ls[i].seat_row;
                int c = toupper(book_ls[i].seat_col) - 'A';
                if (r >= 1 && r <= 30 && c >= 0 && c < 6 && seat_map[r][c] == -1) {
                    seat_map[r][c] = i;
                }
            }
        }
    }

    // ==========================================================
    // 2. CORE SYSTEM LOGIC
    // ==========================================================

    void setup() {
        bookingCount = 0;

        ifstream csvin(CSV_FILE);
        string row, header;
        getline(csvin, header);

        while (getline(csvin, row) && bookingCount < MAX_BOOKING) {
            stringstream ss(row); string r, c, cl;
            getline(ss, book_ls[bookingCount].psg_id, ',');
            getline(ss, book_ls[bookingCount].psg_name, ',');
            getline(ss, r, ','); getline(ss, c, ','); getline(ss, cl, ',');

            try { book_ls[bookingCount].seat_row = stoi(r); } catch (...) {}
            book_ls[bookingCount].seat_col = (c.empty() ? '?' : (char)toupper(c[0]));
            book_ls[bookingCount].seat_class = (cl.empty() ? '?' : (char)toupper(cl[0]));

            book_ls[bookingCount].is_new_reservation = false;
            book_ls[bookingCount].is_assigned = false;
            book_ls[bookingCount].was_dispatched = false;
            bookingCount++;
        }
        rebuildSeatMap();
        auto stop = high_resolution_clock::now();
    }

    void auto_fill() {
        auto start = high_resolution_clock::now();
        int sync = 0;
        for (int i = 0; i < bookingCount; i++) {
            if (!book_ls[i].is_assigned && !book_ls[i].was_dispatched) {
                int r = book_ls[i].seat_row, c = toupper(book_ls[i].seat_col) - 'A';
                if (r >= 1 && r <= 30 && c >= 0 && c < 6 && seat_map[r][c] == -1) {
                    seat_map[r][c] = i;
                    book_ls[i].is_assigned = true;
                    sync++;
                }
            }
        }
        auto stop = high_resolution_clock::now();
        cout << "[System] Auto-fill complete. " << sync << " historical records synchronized.\n";
        cout << "[Performance] Sync Execution Time: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";
    }

    void reserve() {
        char sc;
    bool isValid = false;

    while (!isValid) {
        cout << "Select Class (F = First / B = Business / E = Economy): ";
        cin >> sc;
        sc = toupper(sc);

        if (sc == 'F' || sc == 'B' || sc == 'E') {
            isValid = true;
        } else {
            cout << "[System] Error: Invalid selection. Please enter F, B, or E.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    int rMin = (sc == 'F' ? 1 : (sc == 'B' ? 4 : 11));
    int rMax = (sc == 'F' ? 3 : (sc == 'B' ? 10 : 30));



        cout << "Enter Passenger Name: "; cin.ignore();
        string n; getline(cin, n);

        auto start = high_resolution_clock::now();
        int r, c;
        if (!linearSearchEmptySeat(rMin, rMax, r, c)) {
            cout << "[System] Error: Selected class is fully booked.\n";
            return;
        }


        int id = getMaxPassengerID() + 1;
        book_ls[bookingCount] = {to_string(id), n, r, (char)('A' + c), sc, true, true, false};
        seat_map[r][c] = bookingCount;
        bookingCount++;
        auto stop = high_resolution_clock::now();

        cout << "Reservation Successful! ID: " << id << " | Seat: " << r << (char)('A' + c) << "\n";
        cout << "Reservation Execution Time: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";
        cout << endl;
    }

    void cancel() {
        string t; cout << "Enter Passenger ID to Cancel: "; cin >> t;
        auto start = high_resolution_clock::now();
        int idx = linearSearchByID(t);

        if (idx == -1) {
            cout << "[System] Error: Passenger ID " << t << " not found.\n";
            return;
        }

        for (int i = idx; i < bookingCount - 1; i++) book_ls[i] = book_ls[i + 1];
        bookingCount--;
        rebuildSeatMap();

        auto stop = high_resolution_clock::now();
        cout << "Cancellation confirmed. Seat " << book_ls[idx].seat_row << book_ls[idx].seat_col << " is now available.\n";
        cout << "Cancellation Execution Time: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";
        cout << endl;
    }

    void lookup() {
        string t; cout << "Enter Passenger ID: "; cin >> t;
        auto start = high_resolution_clock::now();
        int idx = linearSearchByID(t);
        auto stop = high_resolution_clock::now();

        if (idx != -1) {
            cout << "\n-----------------------------\n";
            cout << " PASSENGER FOUND\n";
            cout << "-----------------------------\n";
            cout << " Name  : " << book_ls[idx].psg_name << "\n";
            cout << " Seat  : " << book_ls[idx].seat_row << book_ls[idx].seat_col << "\n";
            cout << " Class : " << (book_ls[idx].seat_class == 'F' ? "First" : (book_ls[idx].seat_class == 'B' ? "Business" : "Economy")) << "\n";
            cout << "-----------------------------\n";
        } else {
            cout << "[System] Passenger not found.\n";
        }
        cout << "Lookup (Search) Execution Time: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";
        cout << endl;
    }

    void print_seat() {

        cout << "\n" << string(64, '=') << "\n";
        cout << string(22, ' ') << "FLIGHT SEATING CHART\n";
        cout << string(64, '=') << "\n";
        cout << "      A        B        C        D        E        F\n";

        auto start = high_resolution_clock::now();
        for (int r = 1; r <= 30; r++) {
            if (r == 1) cout << "\n--- [ FIRST CLASS (Rows 1-3) ] ---\n";
            else if (r == 4) cout << "\n--- [ BUSINESS CLASS (Rows 4-10) ] ---\n";
            else if (r == 11) cout << "\n--- [ ECONOMY CLASS (Rows 11-30) ] ---\n";

            cout << setw(2) << r << " ";
            for (int c = 0; c < 6; c++) {
                int idx = seat_map[r][c];
                if (idx != -1) cout << "[" << setw(5) << book_ls[idx].psg_id << "] ";
                else cout << "[------] ";
            }
            cout << "\n";
        }
        auto stop = high_resolution_clock::now();

        cout << string(64, '=') << "\n";
        cout << "Print Seating Execution Time: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";
        cout << endl;
    }

    void print_passenger() {
        auto start = high_resolution_clock::now();
        BookItem m[MAX_BOOKING]; int a = 0;

        for (int r = 1; r <= 30; r++)
            for (int c = 0; c < 6; c++)
                if (seat_map[r][c] != -1) m[a++] = book_ls[seat_map[r][c]];

        bubbleSortByID(m, a);
        auto stop = high_resolution_clock::now();

        cout << "\n" << string(64, '=') << "\n";
        cout << string(23, ' ') << "PASSENGER MANIFEST\n";
        cout << string(64, '=') << "\n";
        cout << left << setw(8) << "Seat" << setw(12) << "ID" << setw(25) << "Name" << "Class\n";
        cout << string(64, '-') << "\n";

        for (int i = 0; i < a; i++) {
            string seat = to_string(m[i].seat_row) + m[i].seat_col;
            cout << left << setw(8) << seat
                 << setw(12) << m[i].psg_id
                 << setw(25) << m[i].psg_name
                 << (m[i].seat_class == 'F' ? "First" : (m[i].seat_class == 'B' ? "Business" : "Economy")) << "\n";
        }
        cout << string(64, '=') << "\n";
        cout << "Manifest Sorting: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";
        cout << endl;
    }

    void dispatch() {
        auto start = high_resolution_clock::now();
        int i = 0;
        while (i < bookingCount) {
            if (book_ls[i].is_new_reservation) {
                for (int j = i; j < bookingCount - 1; j++) book_ls[j] = book_ls[j+1];
                bookingCount--;
            } else {
                if (book_ls[i].is_assigned) book_ls[i].was_dispatched = true;
                book_ls[i].is_assigned = false;
                i++;
            }
        }
        for (int r = 0; r <= 30; r++) for (int c = 0; c < 6; c++) seat_map[r][c] = -1;
        auto stop = high_resolution_clock::now();

        cout << "Dispatch Complete. Session cleared and records marked.\n";
        cout << "Dispatch Execution Time: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";
    }
}
