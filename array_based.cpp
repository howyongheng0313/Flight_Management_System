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

        try { book_ls[bookingCount].seat_row = stoi(seat_row); }
        catch (...) { book_ls[bookingCount].seat_row = 0; }

        book_ls[bookingCount].seat_col   = seat_col.empty()   ? '?' : (char)toupper(seat_col[0]);
        book_ls[bookingCount].seat_class = seat_class.empty() ? '?' : (char)toupper(seat_class[0]);

        bookingCount++;
    }

    csvin.close();
}


    string formatPassengerID(int id) {
        return "ID" + to_string(id);
    }

    int colToIndex(char col) { 
        col = toupper(col); 
        if (col < 'A' || col > 'F') return -1; 
        return col - 'A'; 
    }

    bool isValidClass(char seatClass) {
        seatClass = toupper(seatClass);
        return (seatClass == 'F' || seatClass == 'B' || seatClass == 'E');
    }


    bool isValidRow(char seatClass, int row) {
        seatClass = toupper(seatClass);

        if (seatClass == 'F') return (row >= 1 && row <= 3);
        if (seatClass == 'B') return (row >= 4 && row <= 10);
        if (seatClass == 'E') return (row >= 11 && row <= 30);

        return false;
    }


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


    string classFullName(char seatClass) {
        seatClass = toupper(seatClass);

        if (seatClass == 'F') return "First";
        if (seatClass == 'B') return "Business";
        return "Economy";
    }

    void appendReservation(const string& filename, int pid, const string& name, int seatRow, char seatCol, char seatClass) 
    {
        ofstream out(filename, ios::app);
        out << pid << "," << name << "," << seatRow << "," << seatCol << "," << classFullName(seatClass) << "\n";
    }

    bool deletePassenger(const string& filename, int pidToDelete, int& deletedSeatRow, char& deletedSeatCol) {
        ifstream inFile(filename);
        if (!inFile.is_open()) return false;

        string tempName = filename + ".tmp";
        ofstream outFile(tempName);

        if (!outFile.is_open()) return false;

        string line;
        bool found = false;
        bool isHeader = true;
        deletedSeatRow = -1;
        deletedSeatCol = '?';

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

                        size_t comma2 = line.find(',', comma1 + 1);
                        size_t comma3 = line.find(',', comma2 + 1);
                        size_t comma4 = line.find(',', comma3 + 1);

                        if (comma2 != string::npos && comma3 != string::npos && comma4 != string::npos) {
                            deletedSeatRow = stoi(line.substr(comma2 + 1, comma3 - comma2 - 1));
                            deletedSeatCol = line[comma3 + 1];
                        }

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
    }
    void rebuildSeatMap() {
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
}

        //reservation function
    void reserve() {
    auto start = std::chrono::high_resolution_clock::now();


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

    int assignRow = -1;
    int assignColIdx = -1;

    for (int r = rowMin; r <= rowMax && assignRow == -1; r++) {
        for (int c = 0; c < 6; c++) {
            if (seat_map[r][c] == -1) {
                assignRow = r;
                assignColIdx = c;
                break;
            }
        }
    }

    if (assignRow == -1) {
        cout << "No available seats in " << classFullName(seatClass) << " Class.\n";
        return;
    }

    char assignColChar = char('A' + assignColIdx);
    int nextPassengerID = getMaxPassengerID() + 1;

    appendReservation(CSV_FILE, nextPassengerID, name, assignRow, assignColChar, seatClass);

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
        cout << "[System] Space Complexity: O(R*C) for mapping array.\n";
    };

    printMetrics();
}



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

    void cancel() {
    auto start = std::chrono::high_resolution_clock::now();

    cout << "Enter Passenger ID (e.g. 100001): ";
    string input;
    cin >> input;

    int pid;
    if (!PassengerIDInput(input, pid)) {
        cout << "Invalid Passenger ID format.\n";
        return;
    }

    int idx = -1;
    for (int i = 0; i < bookingCount; i++) {
        try {
            if (!book_ls[i].psg_id.empty() && stoi(book_ls[i].psg_id) == pid) {
                idx = i;
                break;
            }
        } catch (...) {}
    }

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


    rebuildSeatMap();

    cout << "Passenger " << formatPassengerID(pid) << " Reservation is Cancelled.\n";
    cout << "Seat " << seatRow << seatCol << " is now free.\n";

    auto printMetrics = [&]() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        cout << "[System] Execution Time: " << duration.count() << " microseconds.\n";
        cout << "[System] Space Complexity: O(R*C) for mapping array.\n";
    };

    printMetrics();
}

        void lookup() {
    auto start = std::chrono::high_resolution_clock::now();

    auto printMetrics = [&]() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        cout << "[System] Execution Time: " << duration.count() << " microseconds.\n";
        cout << "[System] Space Complexity: O(R*C) for mapping array.\n";
    };

    cout << "Enter Passenger ID (e.g. 100001): ";
    string input;
    cin >> input;

    int pid;
    if (!PassengerIDInput(input, pid)) {
        cout << "Invalid Passenger ID format.\n";
        printMetrics();
        return;
    }

    for (int i = 0; i < bookingCount; i++) {
        if (book_ls[i].psg_id.empty()) continue;

        int idInMem = -1;
        try { idInMem = stoi(book_ls[i].psg_id); } catch (...) { continue; }

        if (idInMem == pid) {
            cout << "Passenger Found - Name: " << book_ls[i].psg_name
                 << " | Seat: " << book_ls[i].seat_row << book_ls[i].seat_col
                 << "\n";
            printMetrics();
            return;
        }
    }

    cout << "Passenger not found.\n";
    printMetrics();
}



    void print_seat() {
            string grid[MAX_ROWS + 1][6]; 

            for (int r = 1; r <= MAX_ROWS; r++)
                for (int c = 0; c < 6; c++) grid[r][c] = "Empty";

            for (const auto& item : book_ls) {
                if (item.psg_id.empty()) continue;
                grid[item.seat_row][item.seat_col - 'A'] = item.psg_id; 
            }

            // Print header
            std::cout << "\n" << string(56, '=') << "\n";
            std::cout << "\t\t   FLIGHT SEATING CHART\n";
            std::cout << string(56, '=') << "\n    ";
            
            // Print column labels
            for (char c = 'A'; c <= 'F'; c++) {
                std::cout << "   " << c << "     "; 
            }
            std::cout << "\n";

            for (int r = 1; r <= MAX_ROWS; r++) {
                if (r == 1) std::cout << "\n--- FIRST CLASS ---\n";
                else if (r == 4) std::cout << "\n--- BUSINESS CLASS ---\n";
                else if (r == 11) std::cout << "\n--- ECONOMY CLASS ---\n"; 

                std::cout << std::setw(2) << r << " ";
                for (int c = 0; c < 6; c++) {
                    std::cout << "[" << std::setw(6) << grid[r][c] << "] ";
                }
                std::cout << "\n";
            }
            std::cout << std::endl;
        }


        void print_passenger() {}

        void teardown() {}
    }
