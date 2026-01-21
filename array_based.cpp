#include <iostream>
#include <fstream>
#include <sstream>
#include "include/array_based.hpp"

using std::getline,
      std::string,
      std::ifstream,
      std::stringstream;

namespace fms::array {
    BookItem book_ls[MAX_BOOKING];

    void setup() {
        string row;
        ifstream csvin("./dataset/flight_passenger_data.csv");

        BookItem *booking = book_ls;
        getline(csvin, row);
        while (getline(csvin, row)) {
            stringstream row_stream(row);
            string seat_row;
            string seat_col;
            string seat_class;

            getline(row_stream, booking->psg_id, ',');
            getline(row_stream, booking->psg_name, ',');
            getline(row_stream, seat_row, ',');
            getline(row_stream, seat_col, ',');
            getline(row_stream, seat_class, ',');

            booking->seat_row = std::stoi(seat_row);
            booking->seat_col = seat_col[0];
            booking->seat_class = seat_class[0];

            booking++;
        }
    }

    void reserve() {}

    void cancel() {}

    void lookup() {}

    void print_seat() {}

    void print_passenger() {}

    void teardown() {}
}
