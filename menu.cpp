#include <iostream>
#include <chrono>
#include "include/menu.hpp"
#include "include/array_based.hpp"
#include "include/linked_list.hpp"

using std::cout,
      std::cin,
      std::endl;

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
                setup_start = high_resolution_clock::now();
                array::setup();
                setup_stop = high_resolution_clock::now();
                next_fn = array_m;
                break;
            case '2':
                setup_start = high_resolution_clock::now();
                llist::setup();
                setup_stop = high_resolution_clock::now();
                next_fn = llist_m;
                break;
            case 'x':
                return;
        }
        cout << "[setup_dur]: " << duration_cast<microseconds>(setup_stop - setup_start).count() << endl;
    }

    void array_m(void *next_m) {
        auto &next_fn = *(void (**)(void *)) next_m;
        cout << "Array-based Implementation Menu:" << endl
             << "    1. Reservation (Allocate Seat)" << endl
             << "    2. Cancellation (Remove Passenger)" << endl
             << "    3. Seat Lookup (Use passengerId)" << endl
             << "    4. Print Seating Chart" << endl
             << "    5. Print Passenger Manifest" << endl
             << "    x. Return Main Menu" << endl << endl;

        char choice;
        while (true) {
            cout << "Select a feature: [   ]\033[3D";
            cin >> choice;

            if ('0' < choice && choice < '6' || choice == 'x') break;
            cout << "\033[91m** Invalid Option ! **\033[m" << endl;
        }
        cout << endl;

        switch (choice) {
            case '1': array::reserve(); break;
            case '2': array::cancel(); break;
            case '3': array::lookup(); break;
            case '4': array::print_seat(); break;
            case '5': array::print_passenger(); break;
            case 'x': next_fn = main_m; return;
        }

        next_fn = array_m;
    }

    void llist_m(void *next_m) {
        auto &next_fn = *(void (**)(void *)) next_m;
        cout << "Linked List-based Implementation Menu:" << endl
             << "    1. Reservation (Allocate Seat)" << endl
             << "    2. Cancellation (Remove Passenger)" << endl
             << "    3. Seat Lookup (Use passengerId)" << endl
             << "    4. Print Seating Chart" << endl
             << "    5. Print Passenger Manifest" << endl
             << "    x. Return Main Menu" << endl << endl;

        char choice;
        while (true) {
            cout << "Select a feature: [   ]\033[3D";
            cin >> choice;

            if ('0' < choice && choice < '6' || choice == 'x') break;
            cout << "\033[91m** Invalid Option ! **\033[m" << endl;
        }
        cout << endl;

        switch (choice) {
            case '1': llist::reserve(); break;
            case '2': llist::cancel(); break;
            case '3': llist::lookup(); break;
            case '4': llist::print_seat(); break;
            case '5': llist::print_passenger(); break;
            case 'x': llist::teardown(); next_fn = main_m; return;
        }

        next_fn = llist_m;
    }
}
