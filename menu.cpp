#include <iostream>
#include "include/menu.hpp"
#include "include/array_based.hpp"
#include "include/linked_list.hpp"

using std::cout,
      std::cin,
      std::endl;

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

        switch (choice) {
            case '1': next_fn = array_m; break;
            case '2': next_fn = llist_m; break;
            case 'x': return;
        }
    }

    void array_m(void *next_m) {
        auto &next_fn = *(void (**)(void *)) next_m;
        cout << "Array-based Implementation Menu:"
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
            case '1': fms::array::reserve(); break;
            case '2': fms::array::cancel(); break;
            case '3': fms::array::lookup(); break;
            case '4': fms::array::print_seat(); break;
            case '5': fms::array::print_passenger(); break;
            case 'x': next_fn = main_m; return;
        }

        next_fn = array_m;
    }

    void llist_m(void *next_m) {
        auto &next_fn = *(void (**)(void *)) next_m;
        cout << "Linked List-based Implementation Menu:"
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
            case '1': fms::llist::reserve(); break;
            case '2': fms::llist::cancel(); break;
            case '3': fms::llist::lookup(); break;
            case '4': fms::llist::print_seat(); break;
            case '5': fms::llist::print_passenger(); break;
            case 'x': next_fn = main_m; return;
        }

        next_fn = llist_m;
    }
}
