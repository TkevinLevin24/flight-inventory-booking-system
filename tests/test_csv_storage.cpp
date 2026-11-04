
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "BookingSystem.h"
#include "CsvStorage.h"

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << "\n";
        std::exit(EXIT_FAILURE);
    }

    std::cout << "PASSED: " << message << "\n";
}

int main() {

    const std::string filename = "test_bookings_temp.csv";

    // TEST 1: Save confirmed and cancelled bookings
    {
        BookingSystem system;

        check(
            system.addFlight("AV900", "BOG", "MIA", 1),
            "Test flight registered"
        );

        check(
            system.addPassenger("P900", "Carlos Perez", "carlos@example.com"),
            "First passenger registered"
        );

        check(
            system.addPassenger("P901", "Ana Gomez", "ana@example.com"),
            "Second passenger registered"
        );

        check(
            system.createBooking("B900", "AV900", "P900"),
            "Confirmed booking created"
        );

        check(
            system.restoreBooking(
                "B901", "AV900", "P901", BookingStatus::Cancelled
            ),
            "Cancelled booking restored"
        );

        check(
            CsvStorage::saveBookings(system, filename),
            "Bookings saved to CSV"
        );
    }

    // TEST 2: Recover bookings in a new system
    {
        BookingSystem restoredSystem;

        check(
            restoredSystem.addFlight("AV900", "BOG", "MIA", 1),
            "Flight recreated for loading"
        );

        check(
            restoredSystem.addPassenger(
                "P900", "Carlos Perez", "carlos@example.com"
            ),
            "First passenger recreated"
        );

        check(
            restoredSystem.addPassenger(
                "P901", "Ana Gomez", "ana@example.com"
            ),
            "Second passenger recreated"
        );

        check(
            CsvStorage::loadBookings(restoredSystem, filename),
            "Bookings loaded from CSV"
        );

        check(
            restoredSystem.getBookingCount() == 2,
            "Both bookings recovered"
        );

        const Booking* confirmed = restoredSystem.findBooking("B900");
        const Booking* cancelled = restoredSystem.findBooking("B901");

        check(
            confirmed != nullptr &&
            confirmed->getStatus() == BookingStatus::Confirmed,
            "Confirmed booking status recovered"
        );

        check(
            cancelled != nullptr &&
            cancelled->getStatus() == BookingStatus::Cancelled,
            "Cancelled booking status recovered"
        );

        const Flight* flight = restoredSystem.findFlight("AV900");

        check(
            flight != nullptr && flight->getAvailableSeats() == 0,
            "Only confirmed booking occupies a seat"
        );
    }

    // TEST 3: Reject an invalid booking status
    {
        std::ofstream file(filename, std::ios::trunc);

        check(
            file.is_open(),
            "Invalid test CSV opened"
        );

        file << "booking_id,flight_number,passenger_id,status\n";
        file << "B999,AV900,P900,UNKNOWN\n";

        file.close();

        BookingSystem system;

        check(
            system.addFlight("AV900", "BOG", "MIA", 1),
            "Invalid test flight registered"
        );

        check(
            system.addPassenger(
                "P900", "Carlos Perez", "carlos@example.com"
            ),
            "Invalid test passenger registered"
        );

        check(
            !CsvStorage::loadBookings(system, filename),
            "Invalid booking status rejected"
        );

        check(
            system.getBookingCount() == 0,
            "Invalid booking not inserted"
        );
    }

    
    // TEST 4: Reject partial CSV loading
    {
        std::ofstream file(filename, std::ios::trunc);

        check(
            file.is_open(),
            "Partial loading test CSV opened"
        );

        file << "booking_id,flight_number,passenger_id,status\n";
        file << "B001,AV900,P900,CONFIRMED\n";
        file << "B002,AV900,P901,CONFIRMED\n";
        file << "B003,AV999,P902,CONFIRMED\n";

        file.close();

        BookingSystem system;

        check(
            system.addFlight("AV900", "BOG", "MIA", 3),
            "Partial loading test flight registered"
        );

        check(
            system.addPassenger(
                "P900", "Carlos Perez", "carlos@example.com"
            ),
            "First passenger registered for partial loading"
        );

        check(
            system.addPassenger(
                "P901", "Ana Gomez", "ana@example.com"
            ),
            "Second passenger registered for partial loading"
        );

        check(
            system.addPassenger(
                "P902", "Laura Martinez", "laura@example.com"
            ),
            "Third passenger registered for partial loading"
        );

        Flight* flight = system.findFlight("AV900");

        check(
            flight != nullptr && flight->getAvailableSeats() == 3,
            "Three seats available before loading"
        );

        check(
            !CsvStorage::loadBookings(system, filename),
            "CSV containing nonexistent flight rejected"
        );

        check(
            system.getBookingCount() == 0,
            "No bookings inserted after failed loading"
        );

        check(
            flight->getAvailableSeats() == 3,
            "Seat inventory unchanged after failed loading"
        );
    }


    // Remove temporary test file
    check(
        std::remove(filename.c_str()) == 0,
        "Temporary CSV removed"
    );

    std::cout
        << "\nALL CSV STORAGE TESTS PASSED!\n";

    return 0;
}
