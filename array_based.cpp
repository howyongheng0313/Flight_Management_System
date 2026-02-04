#include <iostream>
#include <fstream>
#include <sstream>
#include "include/array_based.hpp"
#include <string>
#include <array>
#include <cctype>
#include <limits>
#include <iomanip>
#include <cstdio>
#include <chrono>

using std::getline,
      std::string,
      std::ifstream,
      std::stringstream;
using namespace std;

const int MAX_ROWS = 30;
const int MAX_COLS = 6;

static const string CSV_FILE = "./dataset/flight_passenger_data.csv";

namespace fms::array {

    BookItem book_ls[MAX_BOOKING];

    int bookingCount = 0;
    int seat_map[31][6];

    void setup() {

        bookingCount = 0;

        for (int r = 0; r <= 30; r++)
            for (int c = 0; c < 6; c++)
                seat_map[r][c] = -1;

        for (int i = 0; i < MAX_BOOKING; i++)
            book_ls[i] = BookItem{};

        ifstream csvin(CSV_FILE);

        string row;
        getline(csvin, row);

        while (getline(csvin, row) && bookingCount < MAX_BOOKING) {
            stringstream row_stream(row);

            string seat_row, seat_col, seat_class;

            getline(row_stream, book_ls[bookingCount].psg_id, ',');
            getline(row_stream, book_ls[bookingCount].psg_name, ',');
            getline(row_stream, seat_row, ',');
            getline(row_stream, seat_col, ',');
            getline(row_stream, seat_class, ',');

            try {
                book_ls[bookingCount].seat_row = stoi(seat_row);
            } catch (...) {
                book_ls[bookingCount].seat_row = 0;
            }

            book_ls[bookingCount].seat_col   = seat_col.empty()   ? '?' : (char)toupper(seat_col[0]);
            book_ls[bookingCount].seat_class = seat_class.empty() ? '?' : (char)toupper(seat_class[0]);

            bookingCount++;
        }

        csvin.close();
    }

    //---------------------------------
    // Searching and Sorting Algorithms
    //---------------------------------

    // Linear search by passenger ID
    int linearSearchByID(const string& targetID) {
        for (int i = 0; i < MAX_BOOKING; i++) {
            if (book_ls[i].psg_id == targetID) {
                return i;
            }
        }
        return -1;
    }

    // Linear search by passenger seat
    int linearSearchBySeat(int row, char col) {
        for (int i = 0; i < MAX_BOOKING; i++) {
            if (book_ls[i].psg_id.empty()) continue;

            if (book_ls[i].seat_row == row && book_ls[i].seat_col == col) {
                return i;
            }
        }
        return -1;
    }

    // Bubble sort (Sort based on row and column)
    void bubbleSortByRow(BookItem arr[], int count) {
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - i - 1; j++) {
                bool swap_needed = false;

                if (arr[j].seat_row > arr[j + 1].seat_row) {
                    swap_needed = true;
                } else if (arr[j].seat_row == arr[j + 1].seat_row && arr[j].seat_col > arr[j + 1].seat_col) {
                    swap_needed = true;
                }

                if (swap_needed) {
                    BookItem temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }
    }

    // Linear Search for Reservation
    bool LinearSearchEmptySeat(int rowMin, int rowMax, int& outRow, int& outColIdx) {
        for (int r = rowMin; r <= rowMax; r++) {
            for (int c = 0; c < 6; c++) {
                if (seat_map[r][c] == -1) {
                    outRow = r;
                    outColIdx = c;
                    return true;
                }
            }
        }
        outRow = -1;
        outColIdx = -1;
        return false;
    }

    //---------------------------------
    // Helper Function
    //---------------------------------

    // Format Passenger ID
    string formatPassengerID(int id) {
        return "ID" + to_string(id);
    }

    // Class validation
    bool isValidClass(char seatClass) {
        seatClass = toupper(seatClass);
        return (seatClass == 'F' || seatClass == 'B' || seatClass == 'E');
    }

    // Get maximum passenger ID
    int getMaxPassengerID() {
        ifstream in(CSV_FILE);
        if (!in.is_open()) return 999;

        string line;
        getline(in, line);
        int maxID = 999;

        while (getline(in, line)) {
            if (line.empty()) continue;
            size_t comma = line.find(',');
            if (comma == string::npos) continue;

            try {
                int id = stoi(line.substr(0, comma));
                if (id > maxID) maxID = id;
            } catch (...) {}
        }
        return maxID;
    }

    // Class to full name
    string classFullName(char seatClass) {
        seatClass = toupper(seatClass);

        if (seatClass == 'F') return "First";
        if (seatClass == 'B') return "Business";
        return "Economy";
    }

    // Passenger ID Input
    bool PassengerIDInput(const string& input, int& outID) {
        if (input.empty()) return false;

        string s = input;

        if (s.size() >= 2 &&
            (s[0] == 'I' || s[0] == 'i') &&
            (s[1] == 'D' || s[1] == 'd')) {
            s = s.substr(2);
        }

        if (s.empty()) return false;

        for (char ch : s) {
            if (!isdigit((ch))) return false;
        }

        try {
            outID = stoi(s);
            return true;
        } catch (...) {
            return false;
        }
    }

    //---------------------------------
    // Main Functions
    //---------------------------------

    // Reservation function
    void reserve() {

        char seatClass;
        cout << "Select class (F = First / B = Business / E = Economy): ";
        cin >> seatClass;
        seatClass = toupper(seatClass);

        if (!isValidClass(seatClass)) {
            cout << "Invalid Class.\n";
            return;
        }

        int rowMin, rowMax;
        if (seatClass == 'F') { rowMin = 1; rowMax = 3; }
        else if (seatClass == 'B') { rowMin = 4; rowMax = 10; }
        else { rowMin = 11; rowMax = 30; }

        cout << "Enter Passenger Name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string name;
        getline(cin, name);

        auto start = std::chrono::high_resolution_clock::now();

        int assignRow = -1;
        int assignColIdx = -1;

        if (!LinearSearchEmptySeat(rowMin, rowMax, assignRow, assignColIdx)) {
            cout << "No available seats in " << classFullName(seatClass) << " Class.\n";
            return;
        }

        char assignColChar = char('A' + assignColIdx);
        int nextPassengerID = getMaxPassengerID() + 1;

        auto addReservation = [&](int pid, const string& pname, int seatRow, char seatCol, char seatClass) {
            ofstream out(CSV_FILE, ios::app);
            if (!out.is_open()) return false;
            out << pid << "," << pname << "," << seatRow << "," << seatCol << ","
                << classFullName(seatClass) << "\n";
            return true;
        };

        addReservation(nextPassengerID, name, assignRow, assignColChar, seatClass);

        if (bookingCount >= MAX_BOOKING) {
            cout << "Reservation list is full.\n";
            return;
        }

        book_ls[bookingCount].psg_id = to_string(nextPassengerID);
        book_ls[bookingCount].psg_name = name;
        book_ls[bookingCount].seat_row = assignRow;
        book_ls[bookingCount].seat_col = assignColChar;
        book_ls[bookingCount].seat_class = seatClass;

        seat_map[assignRow][assignColIdx] = bookingCount;
        bookingCount++;

        cout << "Seat " << assignRow << assignColChar << " Reservation successful.\n";
        cout << "Passenger ID: " << formatPassengerID(nextPassengerID) << "\n";

        auto printMetrics = [&]() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            cout << "[System] Execution Time: " << duration.count() << " microseconds.\n";
            cout << "[System] Space Complexity: O(R_class*C) | Extra Space: O(1)\n";
        };

        printMetrics();
        std::cout << std::endl;
    }

    // Cancellation function
    void cancel() {

        cout << "Enter Passenger ID (e.g. 100001): ";
        string input;
        cin >> input;

        int pid;
        if (!PassengerIDInput(input, pid)) {
            cout << "Invalid Passenger ID format.\n";
            return;
        }

        auto start = std::chrono::high_resolution_clock::now();

        auto deletePassenger = [&](const string& filename, int pidToDelete, int& deletedSeatRow, char& deletedSeatCol) -> bool {
            ifstream inFile(filename);
            if (!inFile.is_open()) return false;

            string tempName = filename + ".tmp";
            ofstream outFile(tempName);
            if (!outFile.is_open()) return false;

            string line;
            bool found = false;
            bool isHeader = true;

            while (getline(inFile, line)) {
                if (line.empty()) continue;

                if (isHeader) {
                    isHeader = false;
                    outFile << line << "\n";
                    continue;
                }

                size_t comma1 = line.find(',');
                if (comma1 != string::npos) {
                    try {
                        int pidInFile = stoi(line.substr(0, comma1));
                        if (pidInFile == pidToDelete) {
                            found = true;
                            continue;
                        }
                    } catch (...) {}
                }

                outFile << line << "\n";
            }

            inFile.close();
            outFile.close();

            if (found) {
                remove(filename.c_str());
                rename(tempName.c_str(), filename.c_str());
            } else {
                remove(tempName.c_str());
            }
            return found;
        };

        int idx = linearSearchByID(to_string(pid));

        if (idx == -1) {
            cout << formatPassengerID(pid) << " not found in Reservation List.\n";
            return;
        }

        int seatRow = book_ls[idx].seat_row;
        char seatCol = book_ls[idx].seat_col;

        int deletedSeatRow = -1;
        char deletedSeatCol = '?';
        bool deleted = deletePassenger(CSV_FILE, pid, deletedSeatRow, deletedSeatCol);

        for (int j = idx; j < bookingCount - 1; j++) {
            book_ls[j] = book_ls[j + 1];
        }
        book_ls[bookingCount - 1] = BookItem{};
        bookingCount--;

        auto rebuildSeatMap = [&]() {
            for (int r = 1; r <= 30; r++)
                for (int c = 0; c < 6; c++)
                    seat_map[r][c] = -1;

            for (int i = 0; i < bookingCount; i++) {
                if (book_ls[i].psg_id.empty()) continue;
                int r = book_ls[i].seat_row;
                int c = toupper(book_ls[i].seat_col) - 'A';
                if (r >= 1 && r <= 30 && c >= 0 && c < 6) {
                    seat_map[r][c] = i;
                }
            }
        };

        rebuildSeatMap();

        cout << "Passenger " << formatPassengerID(pid) << " Reservation is Cancelled.\n";
        cout << "Seat " << seatRow << seatCol << " is now free.\n";

        auto printMetrics = [&]() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            cout << "[System] Execution Time: " << duration.count() << " microseconds.\n";
            cout << "[System] Space Complexity: O(R * C) | Extra Space: O(1)\n";
        };

        printMetrics();
        std::cout << std::endl;
    }

    // Lookup function
    void lookup() {

        cout << "Enter Passenger ID (e.g. 100001): ";
        string input;
        cin >> input;

        int pid;

        auto start = std::chrono::high_resolution_clock::now();

        auto printMetrics = [&]() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            cout << "[System] Execution Time: " << duration.count() << " microseconds.\n";
            cout << "[System] Extra Space: O(1) | Search: Linear O(N)\n";
        };

        if (!PassengerIDInput(input, pid)) {
            cout << "Invalid Passenger ID format.\n";
            printMetrics();
            return;
        }

        int idx = linearSearchByID(to_string(pid));

        if (idx == -1) {
            cout << "Passenger not found.\n";
            printMetrics();
            return;
        }

        cout << "Passenger Found - Name: " << book_ls[idx].psg_name
             << " | Seat: " << book_ls[idx].seat_row << book_ls[idx].seat_col
             << "\n";

        printMetrics();
        std::cout << std::endl;
    }

    // Print Seating Chart
    void print_seat() {
        auto start = std::chrono::high_resolution_clock::now();

        cout << "\n" << std::string(64, '=') << "\n";
        cout << "\t\t   FLIGHT SEATING CHART\n";
        cout << std::string(64, '=') << "\n";
        cout << "      A       B       C       D       E       F\n";

        for (int r = 1; r <= 30; r++) {
            if (r == 1)
                cout << "\n--- [ FIRST CLASS (Rows 1-3) ] ---\n";
            else if (r == 4)
                cout << "\n--- [ BUSINESS CLASS (Rows 4-10) ] ---\n";
            else if (r == 11)
                cout << "\n--- [ ECONOMY CLASS (Rows 11-30) ] ---\n";

            std::cout << std::setw(2) << r << " ";
            for (int c = 0; c < 6; c++) {
                int idx = linearSearchBySeat(r, 'A' + c);

                if (idx != -1) {
                    cout << "[" << std::setw(5) << book_ls[idx].psg_id << "] ";
                } else {
                    cout << "[ --- ] ";
                }
            }
            cout << "\n";
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        cout << std::string(64, '=') << "\n";
        cout << "[System] Execution Time: " << duration.count() << " microseconds.\n";
        cout << "[System] Complexity: O(R*C*N) via Linear Search\n";
        cout << std::endl;
    }

    // Print Passenger List
    void print_passenger() {
        auto start = std::chrono::high_resolution_clock::now();

        BookItem manifest[180];
        int count = 0;
        for (int r = 1; r <= 30; r++) {
            for (int c = 0; c < 6; c++) {
                int idx = linearSearchBySeat(r, 'A' + c);
                if (idx != -1) {
                    manifest[count++] = book_ls[idx];
                }
            }
        }

        bubbleSortByRow(manifest, count);

        cout << "\n" << string(64, '=') << "\n";
        cout << "\t\t   PASSENGER MANIFEST\n";
        cout << string(64, '=') << "\n";
        cout << left << setw(8) << "Seat"
             << setw(12) << "ID"
             << setw(25) << "Name" << "Class\n";
        cout << string(64, '-') << "\n";

        for (int i = 0; i < count; i++) {
            const BookItem& p = manifest[i];
            string seat_label = to_string(p.seat_row) + p.seat_col;

            string class_full = (p.seat_class == 'F') ? "First" :
                                (p.seat_class == 'B') ? "Business" : "Economy";

            cout << left << setw(8) << seat_label
                 << setw(12) << p.psg_id
                 << setw(25) << p.psg_name << class_full << "\n";
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        cout << string(64, '=') << endl;
        cout << "[System] Execution Time: " << duration.count() << " microseconds.\n";
        cout << "[System] Complexity: O(n^2) via Bubble Sort\n";
        cout << endl;
    }
} 
