#ifndef CSV_STORAGE_H
#define CSV_STORAGE_H

#include <string>

class BookingSystem;

class CsvStorage {

public:

    static bool saveFlights(
        const BookingSystem& system,
        const std::string& filename
    );

    static bool loadFlights(
        BookingSystem& system,
        const std::string& filename
    );
    static bool savePassengers(
    const BookingSystem& system,
    const std::string& filename
);
static bool loadPassengers(
    BookingSystem& system,
    const std::string& filename
);
static bool saveBookings(
    const BookingSystem& system,
    const std::string& filename
);
static bool loadBookings(
    BookingSystem& system,
    const std::string& filename
);
};

#endif