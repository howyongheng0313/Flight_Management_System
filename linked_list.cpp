#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <limits>
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
    
    struct BookItem {
        string psg_id;
        string psg_name;
        int seat_row;
        char seat_col;
        char seat_class;
        
        bool is_assigned;       
        bool is_new_reservation; 
        bool was_dispatched; 

        BookItem *prev;
        BookItem *next;
    };

    struct Temp {
        string id;
        string name;
        string cls;
        string seat;
        int r;          
        char c;         
        Temp *next;
    };

    BookItem *head = nullptr;
    BookItem *tail = nullptr;
    int bookingCount = 0;
    string seat_display_map[31][6]; 

    // ==========================================
    // 1. ALGORITHMS
    // ==========================================

    BookItem* linearSearchByID(const string& targetID) {
        BookItem* curr = head;
        while (curr) {
            if (curr->psg_id == targetID) {
                return curr;
            }
            curr = curr->next;
        }
        return nullptr;
    }

    bool linearSearchEmptySeat(int minRow, int maxRow, int& outRow, int& outCol) {
        for (int r = minRow; r <= maxRow; r++) {
            for (int c = 0; c < 6; c++) {
                if (seat_display_map[r][c] == "------") {
                    outRow = r;
                    outCol = c;
                    return true; 
                }
            }
        }
        return false; 
    }

    void bubbleSort(Temp* startNode, int criteria) {
        if (!startNode) return;

        bool swapped;
        Temp *ptr1;
        Temp *lptr = nullptr;

        do {
            swapped = false;
            ptr1 = startNode;

            while (ptr1->next != lptr) {
                bool needSwap = false;

                if (criteria == 1) { 
                    try {
                        if (stoi(ptr1->id) > stoi(ptr1->next->id)) needSwap = true;
                    } catch (...) {
                        if (ptr1->id > ptr1->next->id) needSwap = true;
                    }
                } 
                else if (criteria == 2) { 
                    if (ptr1->r > ptr1->next->r) {
                        needSwap = true;
                    } else if (ptr1->r == ptr1->next->r && ptr1->c > ptr1->next->c) {
                        needSwap = true;
                    }
                } 
                else if (criteria == 3) { 
                    if (ptr1->name > ptr1->next->name) needSwap = true;
                }

                if (needSwap) { 
                    std::swap(ptr1->id, ptr1->next->id);
                    std::swap(ptr1->name, ptr1->next->name);
                    std::swap(ptr1->cls, ptr1->next->cls);
                    std::swap(ptr1->seat, ptr1->next->seat);
                    std::swap(ptr1->r, ptr1->next->r);
                    std::swap(ptr1->c, ptr1->next->c);
                    swapped = true;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1;
        } while (swapped);
    }

    // ==========================================
    // 2. HELPER FUNCTIONS
    // ==========================================

    void clear() {
        for(int r=0; r<=30; r++) {
            for(int c=0; c<6; c++) seat_display_map[r][c] = "------";
        }
    }

    void addNode(string id, string name, int r, char c, char cl, bool is_new) {
        BookItem* newNode = new BookItem();
        newNode->psg_id = id;
        newNode->psg_name = name;
        newNode->seat_row = r;
        newNode->seat_col = c;
        newNode->seat_class = cl;
        newNode->is_assigned = false;
        newNode->is_new_reservation = is_new;
        newNode->was_dispatched = false;
        newNode->next = nullptr;

        if (!head) {
            head = tail = newNode;
            head->prev = nullptr;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        bookingCount++;
    }

    int getMaxPassengerID() {
        int max_id = 100000;
        BookItem* curr = head;
        while(curr) {
            try {
                int curr_id = stoi(curr->psg_id);
                if (curr_id > max_id) max_id = curr_id;
            } catch(...) {}
            curr = curr->next;
        }
        return max_id;
    }

    // ==========================================
    // 3. MAIN FUNCTIONS
    // ==========================================

    void setup() {
        teardown(); 
        clear();
        auto start = high_resolution_clock::now();
        ifstream csvin("./dataset/flight_passenger_data.csv");

        if (!csvin.is_open()) {
            cout << "Error opening file." << endl;
            return;
        }

        string row, header;
        getline(csvin, header);

        while (getline(csvin, row)) {
            stringstream ss(row);
            string id, name, s_row, s_col, s_class; 
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, s_row, ',');
            getline(ss, s_col, ',');
            getline(ss, s_class, ',');
            int r = 0;
            try { r = stoi(s_row); } catch (...) {}
            char c = (s_col.empty() ? '?' : s_col[0]);
            char cl = (s_class.empty() ? '?' : s_class[0]);
            addNode(id, name, r, c, cl, false);
        }

        csvin.close();
        auto stop = high_resolution_clock::now();
        cout << "[System] Setup Complete. Loaded " << bookingCount << " records." << endl;
        cout << "[System] Setup Execution Time: " << duration_cast<microseconds>(stop - start).count() << " us" << endl;
        cout << "[System] Space Complexity: O(n) (Nodes + Pointers)" << endl << endl;
    }

    void reserve() {
        char sc;
        bool isValid = false;

        while (!isValid) {
            cout << "Select Class (F = First / B = Business / E = Economy): ";
            // CHANGE: toupper needs std::
            cin >> sc; sc = std::toupper(sc);
            if (sc == 'F' || sc == 'B' || sc == 'E') isValid = true;
            else {
                cout << "Invalid class.\n";
                cin.clear(); 
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }

        int rMin = (sc == 'F' ? 1 : (sc == 'B' ? 4 : 11));
        int rMax = (sc == 'F' ? 3 : (sc == 'B' ? 10 : 30));

        cout << "Enter Passenger Name: "; cin.ignore();
        string n; getline(cin, n);

        auto start = high_resolution_clock::now();
        int foundRow, foundCol;
        bool seatFound = linearSearchEmptySeat(rMin, rMax, foundRow, foundCol);

        if (!seatFound) {
            cout << "[System] Error: Selected class is fully booked.\n";
            return;
        }

        int newId = getMaxPassengerID() + 1;
        addNode(to_string(newId), n, foundRow, (char)('A' + foundCol), sc, true);

        tail->is_assigned = true;
        seat_display_map[foundRow][foundCol] = to_string(newId);
        
        auto stop = high_resolution_clock::now();
        cout << "Reservation Successful! ID: " << newId << " | Seat: " << foundRow << (char)('A' + foundCol) << endl;
        cout << "Reservation Execution Time: " << duration_cast<microseconds>(stop - start).count() << " us" << endl << endl;
    }

    void cancel() {
        string t; 
        cout << "Enter Passenger ID to Cancel: "; 
        cin >> t;

        auto start = high_resolution_clock::now();

        BookItem* targetNode = linearSearchByID(t);

        if (targetNode != nullptr) {
            if (targetNode->is_assigned) {
                int c = std::toupper(targetNode->seat_col) - 'A';
                if(targetNode->seat_row >=1 && targetNode->seat_row <= 30 && c >=0 && c < 6)
                    seat_display_map[targetNode->seat_row][c] = "------";
            }

            if (targetNode->prev) targetNode->prev->next = targetNode->next;
            else head = targetNode->next;

            if (targetNode->next) targetNode->next->prev = targetNode->prev;
            else tail = targetNode->prev;

            delete targetNode;
            bookingCount--;

            auto stop = high_resolution_clock::now();
            cout << "Cancellation confirmed for ID " << t << ".\n";
            cout << "Cancellation Execution Time: " << duration_cast<microseconds>(stop - start).count() << " us\n\n";
        } else {
            cout << "[System] Error: Passenger ID not found.\n";
        }
    }

    void auto_fill() {
        auto start = high_resolution_clock::now();
        int sync = 0;
        BookItem* curr = head;

        while (curr) {
            if (!curr->is_assigned && !curr->was_dispatched) {
                int r = curr->seat_row;
                int c = std::toupper(curr->seat_col) - 'A';

                if (r >= 1 && r <= 30 && c >= 0 && c < 6) {
                    if (seat_display_map[r][c] == "------") {
                        seat_display_map[r][c] = curr->psg_id;
                        curr->is_assigned = true;
                        sync++;
                    }
                }
            }
            curr = curr->next;
        }

        auto stop = high_resolution_clock::now();
        cout << "[System] Auto-fill complete. " << sync << " records synchronized.\n";
        cout << "[Performance] Sync Execution Time: " << duration_cast<microseconds>(stop - start).count() << " us\n\n";
    }

    void lookup() {
        string t; 
        cout << "Enter Passenger ID: "; 
        cin >> t;
        auto start = high_resolution_clock::now();
        BookItem* foundNode = linearSearchByID(t);
        auto stop = high_resolution_clock::now();

        if (foundNode) {
            cout << "\n-----------------------------\n";
            cout << " PASSENGER FOUND\n";
            cout << "-----------------------------\n";
            cout << " Name  : " << foundNode->psg_name << "\n";
            cout << " Seat  : " << foundNode->seat_row << foundNode->seat_col << "\n";
            string cls = (foundNode->seat_class == 'F' ? "First" : (foundNode->seat_class == 'B' ? "Business" : "Economy"));
            cout << " Class : " << cls << "\n";
            cout << "-----------------------------\n";
            cout << "Lookup Execution Time: " << duration_cast<microseconds>(stop - start).count() << " us\n\n";
        } else {
            cout << "[System] Passenger not found.\n\n";
        }
    }

    void print_seat() {
        cout << "\n" << string(64, '=') << "\n";
        cout << "\t\t   FLIGHT SEATING CHART\n";
        cout << string(64, '=') << "\n";
        cout << "      A       B       C       D       E       F\n";
        auto start = high_resolution_clock::now();

        for (int r = 1; r <= 30; r++) {
            if (r == 1) cout << "\n--- [ FIRST CLASS (Rows 1-3) ] ---\n";
            else if (r == 4) cout << "\n--- [ BUSINESS CLASS (Rows 4-10) ] ---\n";
            else if (r == 11) cout << "\n--- [ ECONOMY CLASS (Rows 11-30) ] ---\n";
            cout << std::setw(2) << r << " ";

            for (int c = 0; c < 6; c++) {
                string val = seat_display_map[r][c];
                if (val != "------") cout << "[" << std::setw(5) << val << "] ";
                else cout << "[" << val << "] ";
            }
            cout << "\n";
        }

        auto stop = high_resolution_clock::now();
        cout << string(64, '=') << "\n";
        cout << "Print Seating Execution Time: " << duration_cast<microseconds>(stop - start).count() << " us\n\n";
    }

    void print_passenger() {
        cout << "Do you want to sort by:\n";
        cout << "1. Passenger ID\n";
        cout << "2. Seat\n";
        cout << "3. Passenger's Name\n";
        cout << "Select option: ";
        
        int criteria;
        cin >> criteria;

        if (criteria < 1 || criteria > 3) {
            cout << "Invalid option. Defaulting to Passenger ID.\n";
            criteria = 1;
        }

        auto start = high_resolution_clock::now(); 
        Temp* mHead = nullptr;
        BookItem* curr = head;

        while(curr) {
            if(curr->is_assigned) {
                Temp* n = new Temp{
                    curr->psg_id, 
                    curr->psg_name, 
                    string(1, curr->seat_class), 
                    to_string(curr->seat_row) + curr->seat_col, 
                    curr->seat_row,
                    curr->seat_col, 
                    nullptr
                };
                n->next = mHead;
                mHead = n;
            }
            curr = curr->next;
        }

        bubbleSort(mHead, criteria);
        
        auto stop = high_resolution_clock::now();

        string sortType = (criteria == 1 ? "ID" : (criteria == 2 ? "Seat" : "Name"));

        cout << "\n" << string(64, '=') << "\n";
        cout << "\t\t   PASSENGER MANIFEST (Sorted by " << sortType << ")\n";
        cout << string(64, '=') << "\n";
        cout << std::left << std::setw(8) << "Seat" << std::setw(12) << "ID" << std::setw(25) << "Name" << "Class\n";
        cout << string(64, '-') << "\n";

        Temp* mCurr = mHead;
        while(mCurr) {
            string className = (mCurr->cls == "F" ? "First" : (mCurr->cls == "B" ? "Business" : "Economy"));
            cout << std::left << std::setw(8) << mCurr->seat 
                 << std::setw(12) << mCurr->id 
                 << std::setw(25) << mCurr->name 
                 << className << "\n";
            Temp* toDelete = mCurr;
            mCurr = mCurr->next;
            delete toDelete;
        }

        cout << string(64, '=') << "\n";
        cout << "Manifest Sorting & Print Time: " << duration_cast<microseconds>(stop - start).count() << " us\n\n";
    }

    void dispatch() {
        auto start = high_resolution_clock::now();
        BookItem* curr = head;
        while (curr) {
            BookItem* nextNode = curr->next; 
            if (curr->is_new_reservation) {
                if (curr->prev) curr->prev->next = curr->next;
                else head = curr->next;
                if (curr->next) curr->next->prev = curr->prev;
                else tail = curr->prev;
                delete curr;
                bookingCount--;
            } else {
                if (curr->is_assigned) {
                    curr->was_dispatched = true;
                    curr->is_assigned = false;
                }
            }
            curr = nextNode;
        }
        clear();
        auto stop = high_resolution_clock::now();
        cout << "Dispatch Complete. Session cleared.\n";
        cout << "Dispatch Execution Time: " << duration_cast<microseconds>(stop - start).count() << " us\n\n";
    }

    void teardown() {
        BookItem* curr = head;
        while (curr) {
            BookItem* next = curr->next;
            delete curr;
            curr = next;
        }
        head = nullptr;
        tail = nullptr;
        bookingCount = 0;
    }
}