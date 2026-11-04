#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "Flight.h"
#include "Passenger.h"
#include "Booking.h"

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << "\n";
        std::exit(EXIT_FAILURE);
    }

    std::cout << "PASSED: " << message << "\n";
}

int main() {

    // TEST 1: Create a booking
    Flight flight("AV245", "BOG", "MIA", 1);

    Passenger passenger(
        "P001",
        "Carlos Perez",
        "carlos@example.com"
    );

    Booking booking("B001", flight, passenger);

    check(
        !booking.isConfirmed(),
        "New booking starts unconfirmed"
    );

    // TEST 2: Confirm booking
    check(
        booking.confirmBooking(),
        "Booking can be confirmed"
    );

    check(
        booking.isConfirmed(),
        "Booking status is confirmed"
    );

    check(
        flight.getAvailableSeats() == 0,
        "Confirmed booking reduces availability"
    );

    // TEST 3: Prevent duplicate confirmation
    check(
        !booking.confirmBooking(),
        "Duplicate confirmation is rejected"
    );

    check(
        flight.getAvailableSeats() == 0,
        "Duplicate confirmation does not change availability"
    );

    // TEST 4: Prevent overbooking
    Passenger passenger2(
        "P002",
        "Maria Lopez",
        "maria@example.com"
    );

    Booking booking2("B002", flight, passenger2);

    check(
        !booking2.confirmBooking(),
        "Booking on a full flight is rejected"
    );

    // TEST 5: Cancel booking
    check(
        booking.cancelBooking(),
        "Confirmed booking can be cancelled"
    );

    check(
        !booking.isConfirmed(),
        "Cancelled booking is no longer confirmed"
    );

    check(
        flight.getAvailableSeats() == 1,
        "Cancellation restores availability"
    );

    // TEST 6: Prevent duplicate cancellation
    check(
        !booking.cancelBooking(),
        "Duplicate cancellation is rejected"
    );

    check(
        flight.getAvailableSeats() == 1,
        "Duplicate cancellation does not change availability"
    );

    // TEST 7: Reject invalid booking ID
    bool exceptionThrown = false;

    try {
        Booking invalidBooking("", flight, passenger);
    }
    catch (const std::invalid_argument&) {
        exceptionThrown = true;
    }

    check(
        exceptionThrown,
        "Empty booking ID is rejected"
    );

    std::cout << "\nALL BOOKING TESTS PASSED!\n";

    return 0;
}