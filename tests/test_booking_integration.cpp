#include <cstdlib>
#include <iostream>

#include "BookingSystem.h"

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << "\n";
        std::exit(EXIT_FAILURE);
    }

    std::cout << "PASSED: " << message << "\n";
}

int main() {

    BookingSystem system;

    // TEST 1: Initial state
    check(
        system.getBookingCount() == 0,
        "System starts with zero bookings"
    );

    // Register one flight with two seats
    check(
        system.addFlight("AV245", "BOG", "MIA", 2),
        "Flight registered successfully"
    );

    // Register passengers
    check(
        system.addPassenger(
            "P001",
            "Carlos Perez",
            "carlos@example.com"
        ),
        "First passenger registered"
    );

    check(
        system.addPassenger(
            "P002",
            "Maria Lopez",
            "maria@example.com"
        ),
        "Second passenger registered"
    );

    check(
        system.addPassenger(
            "P003",
            "Juan Gomez",
            "juan@example.com"
        ),
        "Third passenger registered"
    );

    // TEST 2: Create first booking
    check(
        system.createBooking("B001", "AV245", "P001"),
        "First booking confirmed"
    );

    check(
        system.getBookingCount() == 1,
        "Booking count increased to one"
    );

    const Booking* booking1 = system.findBooking("B001");

    check(
        booking1 != nullptr,
        "First booking found"
    );

    check(
        booking1->isConfirmed(),
        "First booking status is confirmed"
    );

    Flight* flight = system.findFlight("AV245");

    check(
        flight != nullptr,
        "Flight found after booking"
    );

    check(
        flight->getAvailableSeats() == 1,
        "First booking reduces availability"
    );

    // TEST 3: Prevent duplicate booking IDs
    check(
        !system.createBooking("B001", "AV245", "P002"),
        "Duplicate booking ID rejected"
    );

    check(
        system.getBookingCount() == 1,
        "Duplicate does not increase booking count"
    );

    // TEST 4: Create second booking
    check(
        system.createBooking("B002", "AV245", "P002"),
        "Second booking confirmed"
    );

    check(
        flight->getAvailableSeats() == 0,
        "Flight is fully booked"
    );

    // TEST 5: Prevent overbooking
    check(
        !system.createBooking("B003", "AV245", "P003"),
        "Overbooking prevented"
    );

    check(
        system.getBookingCount() == 2,
        "Rejected booking is not stored"
    );

    // TEST 6: Cancel a booking
    check(
        system.cancelBooking("B001"),
        "First booking cancelled"
    );

    check(
        !booking1->isConfirmed(),
        "Cancelled booking status updated"
    );

    check(
        flight->getAvailableSeats() == 1,
        "Cancellation restores availability"
    );

    // TEST 7: Prevent duplicate cancellation
    check(
        !system.cancelBooking("B001"),
        "Duplicate cancellation rejected"
    );

    check(
        flight->getAvailableSeats() == 1,
        "Duplicate cancellation does not release another seat"
    );

    // TEST 8: Book released seat
    check(
        system.createBooking("B003", "AV245", "P003"),
        "Released seat can be booked again"
    );

    check(
        flight->getAvailableSeats() == 0,
        "Availability returns to zero"
    );

    check(
        system.getBookingCount() == 3,
        "Booking history contains three records"
    );

    // TEST 9: Reject invalid references
    check(
        !system.createBooking("B004", "AV999", "P001"),
        "Booking with nonexistent flight rejected"
    );

    check(
        !system.createBooking("B005", "AV245", "P999"),
        "Booking with nonexistent passenger rejected"
    );

    check(
        !system.createBooking("", "AV245", "P001"),
        "Empty booking ID rejected"
    );

    // TEST 10: Search and cancel nonexistent bookings
    check(
        system.findBooking("B999") == nullptr,
        "Nonexistent booking returns nullptr"
    );

    check(
        !system.cancelBooking("B999"),
        "Cancellation of nonexistent booking rejected"
    );

    check(
        system.getBookingCount() == 3,
        "Invalid operations do not change booking count"
    );

    // TEST 11: Prevent duplicate active bookings

check(
    !system.createBooking("B006", "AV245", "P002"),
    "Duplicate active booking for same passenger and flight rejected"
);

check(
    system.getBookingCount() == 3,
    "Duplicate active booking does not increase booking count"
);

// TEST 12: Cancelled passenger can book again

check(
    system.createBooking("B007", "AV245", "P001") == false,
    "Cancelled passenger cannot book while flight is full"
);

check(
    system.cancelBooking("B002"),
    "Second booking cancelled to release a seat"
);

check(
    system.createBooking("B007", "AV245", "P001"),
    "Passenger with cancelled booking can book again"
);

check(
    system.findBooking("B007") != nullptr,
    "New booking exists"
);

check(
    system.findBooking("B007")->isConfirmed(),
    "New booking is confirmed"
);

check(
    system.getBookingCount() == 4,
    "Booking history includes cancelled and new bookings"
);

check(
    flight->getAvailableSeats() == 0,
    "Availability remains consistent after rebooking"
);

    std::cout
        << "\nALL BOOKING INTEGRATION TESTS PASSED!\n";

    return 0;
}