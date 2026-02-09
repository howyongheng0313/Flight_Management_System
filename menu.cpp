#include <iostream>
#include <chrono>
#include "include/menu.hpp"
#include "include/array_based.hpp"
#include "include/linked_list.hpp"

using std::cout,
      std::cin,
      std::endl,
      std::string;

using namespace std::chrono;

namespace fms::menu {
    void main_m(void *next_m) {
        auto &next_fn = *(void (**)(void *)) next_m;
        cout << "---------------------------------------------------------------" << endl
             << "                        FLIGHT MANAGEMENT" << endl
             << "---------------------------------------------------------------" << endl
             << "Select data structure component:" << endl
             << "    1. Array-based Implementation" << endl
             << "    2. Linked List-Based Implementation" << endl
             << "    x. Exit Program" << endl << endl;

        char choice;
        while (true) {
            cout << "Select a data structure: [   ]\033[3D";
            cin >> choice;

            if ('0' < choice && choice < '3' || choice == 'x') break;
            cout << "\033[91m** Invalid Option ! **\033[m" << endl;
        }
        cout << endl;

        time_point<high_resolution_clock> setup_start;
        time_point<high_resolution_clock> setup_stop;
        switch (choice) {
            case '1':
                array::setup();
                next_fn = array_m;
                break;
            case '2':
                llist::setup();
                next_fn = llist_m;
                break;
            case 'x':
                return;
        }
    }

    void array_m(void *next_m) {
        auto &next_fn = *(void (**)(void *)) next_m;

        char choice = base_m("Array-based Implementation Menu:");
        switch (choice) {
            case '1': array::reserve(); break;
            case '2': array::cancel(); break;
            case '3': array::auto_fill(); break;
            case '4': array::lookup(); break;
            case '5': array::print_seat(); break;
            case '6': array::print_passenger(); break;
            case '7': array::dispatch(); break;
            case 'x': next_fn = main_m; return;
        }

        next_fn = array_m;
    }

    void llist_m(void *next_m) {
        auto &next_fn = *(void (**)(void *)) next_m;

        char choice = base_m("Linked List-based Implementation Menu:");
        switch (choice) {
            case '1': llist::reserve(); break;
            case '2': llist::cancel(); break;
            case '3': llist::auto_fill(); break;
            case '4': llist::lookup(); break;
            case '5': llist::print_seat(); break;
            case '6': llist::print_passenger(); break;
            case '7': llist::dispatch(); break;
            case 'x': llist::teardown(); next_fn = main_m; return;
        }

        next_fn = llist_m;
    }

    char base_m(string title) {
        cout << title << endl
             << "    1. Reservation (Allocate Seat)" << endl
             << "    2. Cancellation (Remove Passenger)" << endl
             << "    3. Auto Fill Seating" << endl
             << "    4. Seat Lookup (Use passengerId)" << endl
             << "    5. Print Seating Chart" << endl
             << "    6. Print Passenger Manifest" << endl
             << "    7. Dispatch Now" << endl
             << "    x. Return Main Menu (Reset)" << endl << endl;

        char choice;
        while (true) {
            cout << "Select a feature: [   ]\033[3D";
            cin >> choice;

            if ('0' < choice && choice < '8' || choice == 'x') break;
            cout << "\033[91m** Invalid Option ! **\033[m" << endl;
        }
        cout << endl;
        return choice;
    }
}
