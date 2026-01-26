#include <iostream>
#include <string>
#include <array>
#include <cctype>
#include <limits>
#include <fstream>
#include <iomanip>
#include <cstdio>

using namespace std;

const int MAX_ROWS = 30;
const int MAX_COLS = 6;


const string DATA_FILE = "C:\\Users\\randy\\OneDrive\\Documents\\APU Y2S2\\Data Structure Assignment\\flight_passenger_data.csv";

struct Passenger {
    int passengerID = -1;
    string name = "";
    char seatClass = 'E';  
};

struct SeatCell {
    bool isOccupied = false;
    Passenger p;
};

using SeatGrid = array<array<SeatCell, MAX_COLS>, MAX_ROWS>;


int colToIndex(char col) {
    col = static_cast<char>(toupper(static_cast<unsigned char>(col)));
    if (col < 'A' || col > 'F') return -1;
    return col - 'A';
}
char indexToCol(int idx) {
    return static_cast<char>('A' + idx);
}

string formatPassengerID(int id) {
    return "ID" + to_string(id);
}


bool parsePassengerIDInput(const string& input, int& outID) {
    if (input.empty()) return false;

    string s = input;
    if (s.size() >= 2 && (s[0] == 'I' || s[0] == 'i') && (s[1] == 'D' || s[1] == 'd')) {
        s = s.substr(2);
    }

    if (s.empty()) return false;

    for (char ch : s) {
        if (!isdigit(static_cast<unsigned char>(ch))) return false;
    }

    try {
        outID = stoi(s);
        return true;
    } catch (...) {
        return false;
    }
}

bool isValidClass(char seatClass) {
    seatClass = static_cast<char>(toupper(static_cast<unsigned char>(seatClass)));
    return (seatClass == 'F' || seatClass == 'B' || seatClass == 'E');
}


bool isValidRow(char seatClass, int row) {
    seatClass = static_cast<char>(toupper(static_cast<unsigned char>(seatClass)));

    if (seatClass == 'F') return (row >= 1 && row <= 3);
    if (seatClass == 'B') return (row >= 4 && row <= 10);
    if (seatClass == 'E') return (row >= 11 && row <= 30);

    return false;
}


int getMaxPassengerID(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return 999;

    string line;
    getline(file, line); 
    
    int maxID = 999;
    while (getline(file, line)) {
        size_t commaPos = line.find(',');
        if (commaPos != string::npos) {
            try {
                int id = stoi(line.substr(0, commaPos));
                if (id > maxID) maxID = id;
            } catch (...) {}
        }
    }
    
    return maxID;
}

string classFullName(char seatClass) {
    seatClass = static_cast<char>(toupper(static_cast<unsigned char>(seatClass)));

    if (seatClass == 'F') return "First";
    if (seatClass == 'B') return "Business";
    return "Economy";
}

void appendReservation(const string& filename, int pid, const string& name,int seatRow, char seatCol, char seatClass) 
{
    ofstream out(filename, ios::app); 
    if (!out.is_open()) {
        cout << " Unable to open file " << filename << "\n";
        return;
    }

    out << pid << "," << name << "," << seatRow << "," << seatCol << "," << classFullName(seatClass) << "\n";
}

bool deletePassengerFromCSV(const string& filename, int pidToDelete) {
    ifstream inFile(filename);

    string tempFile = filename + ".tmp";
    ofstream outFile(tempFile, ios::out);

    string line;
    bool found = false;
    bool firstLine = true;

    while (getline(inFile, line)) {
        if (line.empty()) continue;

        // Always keep header
        if (firstLine) {
            firstLine = false;
            outFile << line << "\n";
            continue;
        }

        size_t commaPos = line.find(',');
        if (commaPos == string::npos) {
            outFile << line << "\n";
            continue;
        }

        string idStr = line.substr(0, commaPos);

        try {
            int pidInFile = stoi(idStr);

            if (pidInFile == pidToDelete) {
                found = true;
                continue;
            }

        } catch (...) {
        }

        outFile << line << "\n";
    }

    inFile.close();
    outFile.close();

    remove(filename.c_str());
    rename(tempFile.c_str(), filename.c_str());

    return found;
}



// 1) Reservation
void reservation(SeatGrid& grid, int& nextPassengerID) {

    char seatClass;
    cout << "Select class (F = First / B = Business / E = Economy): ";
    cin >> seatClass;
    seatClass = static_cast<char>(toupper(static_cast<unsigned char>(seatClass)));

    if (!isValidClass(seatClass)) {
        cout << "Invalid class.\n";
        return;
    }

    int rowMin, rowMax;
    if (seatClass == 'F') { rowMin = 1; rowMax = 3; }
    else if (seatClass == 'B') { rowMin = 4; rowMax = 10; }
    else { rowMin = 11; rowMax = 30; }

    int row;
    cout << "Select row (" << rowMin << " ~ " << rowMax << "): ";
    cin >> row;

    if (!isValidRow(seatClass, row)) {
        cout << "Invalid row for selected class.\n";
        return;
    }

    // Show available columns in that row
    cout << "Available column = [";
    bool anyAvailable = false;
    int r = row - 1;

    for (int c = 0; c < MAX_COLS; c++) {
        if (!grid[r][c].isOccupied) {
            if (anyAvailable) cout << ", ";
            cout << indexToCol(c);
            anyAvailable = true;
        }
    }
    cout << "]\n";

    if (!anyAvailable) {
        cout << "No seats available in row " << row << ".\n";
        return;
    }

    char col;
    cout << "Select a column: ";
    cin >> col;
    col = static_cast<char>(toupper(static_cast<unsigned char>(col)));

    int cIndex = colToIndex(col);
    if (cIndex == -1) {
        cout << "Invalid column. Must be A-F.\n";
        return;
    }

    if (grid[r][cIndex].isOccupied) {
        cout << "Seat " << row << col << " is already occupied.\n";
        return;
    }

    cout << "Enter Passenger Name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string name;
    getline(cin, name);

    // Allocate seat
    grid[r][cIndex].isOccupied = true;
    grid[r][cIndex].p.passengerID = nextPassengerID++;
    grid[r][cIndex].p.name = name;
    grid[r][cIndex].p.seatClass = seatClass;

    cout << "Seat updated. Reservation successful.\n";
    cout << "Passenger ID: " << formatPassengerID(grid[r][cIndex].p.passengerID) << "\n";


    appendReservation(DATA_FILE,
                       grid[r][cIndex].p.passengerID,
                       grid[r][cIndex].p.name,
                       row,
                       col,
                       seatClass);

}

void cancellation(SeatGrid& grid) {
    cout << "Enter Passenger ID (e.g. 100001): ";

    string input;
    cin >> input;

    int pid;
    if (!parsePassengerIDInput(input, pid)) {
        cout << "Invalid Passenger ID.\n";
        return;
    }

    bool freedSeat = false;

    bool deleted = deletePassengerFromCSV(DATA_FILE, pid);

     if (deleted) {
        cout << "Passenger " << formatPassengerID(pid) << " Reservation is Cancelled.\n";
    } else {
        cout << formatPassengerID(pid) << " not found in Reservation List.\n";
    }

    for (int r = 0; r < MAX_ROWS; r++) {
        for (int c = 0; c < MAX_COLS; c++) {
            if (grid[r][c].isOccupied && grid[r][c].p.passengerID == pid) {

                int seatRow = r + 1;
                char seatCol = indexToCol(c);

                grid[r][c].isOccupied = false;
                grid[r][c].p = Passenger{};

                cout << "Seat " << seatRow << seatCol << " is now free.\n";
                freedSeat = true;
                break;
            }
        }
        if (freedSeat) break;
    }

}



int main() {
    SeatGrid grid{}; 

    int maxID = getMaxPassengerID(DATA_FILE);
    int nextPassengerID = maxID + 1;
                  

    while (true) {
        cout << "    1. Reservation\n";
        cout << "    2. Cancellation\n";
        cout << "    3. Seat Lookup (Use passengerId)\n";
        cout << "    4. Print Seating Chart\n";
        cout << "    5. Print Passenger Manifest\n";
        cout << "    6. Exit\n";
        cout << "Select a feature: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            reservation(grid, nextPassengerID);
        } else if (choice == 2) {
            cancellation(grid);
        } else if (choice == 6) {
            cout << "Thank You for Using.\n";
            break;
        } else {
            cout << "Invalid choice.\n";
        }
    }

    return 0;
}