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

    // TEST 1: Empty system
    check(
        system.getFlightCount() == 0,
        "System starts with zero flights"
    );

    // TEST 2: Register flights
    check(
        system.addFlight("AV245", "BOG", "MIA", 180),
        "First flight registered successfully"
    );

    check(
        system.addFlight("AV120", "BOG", "MAD", 250),
        "Second flight registered successfully"
    );

    check(
        system.getFlightCount() == 2,
        "Flight count is correct"
    );

    // TEST 3: Prevent duplicate flight numbers
    check(
        !system.addFlight("AV245", "MDE", "JFK", 150),
        "Duplicate flight number rejected"
    );

    check(
        system.getFlightCount() == 2,
        "Duplicate does not increase flight count"
    );

    // TEST 4: Find an existing flight
    Flight* flight = system.findFlight("AV245");

    check(
        flight != nullptr,
        "Existing flight found"
    );

    check(
        flight->getOrigin() == "BOG",
        "Flight origin is correct"
    );

    check(
        flight->getDestination() == "MIA",
        "Flight destination is correct"
    );

    check(
        flight->getAvailableSeats() == 180,
        "Flight availability is correct"
    );

    // TEST 5: Find a nonexistent flight
    check(
        system.findFlight("AV999") == nullptr,
        "Nonexistent flight returns nullptr"
    );

    // TEST 6: Reject invalid flights
    check(
        !system.addFlight("", "BOG", "MIA", 180),
        "Empty flight number rejected"
    );

    check(
        !system.addFlight("AV300", "", "MIA", 180),
        "Empty origin rejected"
    );

    check(
        !system.addFlight("AV301", "BOG", "", 180),
        "Empty destination rejected"
    );

    check(
        !system.addFlight("AV302", "BOG", "MIA", 0),
        "Zero seat capacity rejected"
    );

    check(
        !system.addFlight("AV303", "BOG", "MIA", -10),
        "Negative seat capacity rejected"
    );

    check(
        system.getFlightCount() == 2,
        "Invalid flights do not change flight count"
    );

    // TEST 7: Passenger management

check(
    system.getPassengerCount() == 0,
    "System starts with zero passengers"
);

// Register passengers
check(
    system.addPassenger(
        "P001",
        "Carlos Perez",
        "carlos@example.com"
    ),
    "First passenger registered successfully"
);

check(
    system.addPassenger(
        "P002",
        "Maria Lopez",
        "maria@example.com"
    ),
    "Second passenger registered successfully"
);

check(
    system.getPassengerCount() == 2,
    "Passenger count is correct"
);

// Prevent duplicate passenger IDs
check(
    !system.addPassenger(
        "P001",
        "Another Passenger",
        "another@example.com"
    ),
    "Duplicate passenger ID rejected"
);

check(
    system.getPassengerCount() == 2,
    "Duplicate passenger does not increase count"
);

// Find an existing passenger
const Passenger* passenger = system.findPassenger("P001");

check(
    passenger != nullptr,
    "Existing passenger found"
);

check(
    passenger->getPassengerId() == "P001",
    "Passenger ID is correct"
);

check(
    passenger->getName() == "Carlos Perez",
    "Passenger name is correct"
);

check(
    passenger->getEmail() == "carlos@example.com",
    "Passenger email is correct"
);

// Search for a nonexistent passenger
check(
    system.findPassenger("P999") == nullptr,
    "Nonexistent passenger returns nullptr"
);

// Reject invalid passenger data
check(
    !system.addPassenger(
        "",
        "Juan Gomez",
        "juan@example.com"
    ),
    "Empty passenger ID rejected"
);

check(
    !system.addPassenger(
        "P003",
        "",
        "juan@example.com"
    ),
    "Empty passenger name rejected"
);

check(
    !system.addPassenger(
        "P004",
        "Juan Gomez",
        ""
    ),
    "Empty passenger email rejected"
);


// Verify that invalid operations did not modify the system
check(
    system.getPassengerCount() == 2,
    "Invalid passengers do not change count"
);

// TEST 8: Restore cancelled booking when flight is full
{
    BookingSystem restoreSystem;

    check(
        restoreSystem.addFlight("AV900", "BOG", "MIA", 1),
        "Restore test flight registered"
    );

    check(
        restoreSystem.addPassenger(
            "P900",
            "Test Passenger",
            "test@example.com"
        ),
        "Restore test passenger registered"
    );

    check(
        restoreSystem.addPassenger(
            "P901",
            "Cancelled Passenger",
            "cancelled@example.com"
        ),
        "Cancelled test passenger registered"
    );

    // Restore a confirmed booking
    check(
        restoreSystem.restoreBooking(
            "B900",
            "AV900",
            "P900",
            BookingStatus::Confirmed
        ),
        "Confirmed booking restored successfully"
    );

    Flight* restoreFlight = restoreSystem.findFlight("AV900");

    check(
        restoreFlight != nullptr,
        "Restore test flight found"
    );

    check(
        restoreFlight->getAvailableSeats() == 0,
        "Confirmed booking occupies the only available seat"
    );

    // Restore a cancelled booking while the flight is full
    check(
        restoreSystem.restoreBooking(
            "B901",
            "AV900",
            "P901",
            BookingStatus::Cancelled
        ),
        "Cancelled booking restored even when flight is full"
    );

    const Booking* cancelledBooking =
        restoreSystem.findBooking("B901");

    check(
        cancelledBooking != nullptr,
        "Cancelled booking found in history"
    );

    check(
        cancelledBooking->getStatus() == BookingStatus::Cancelled,
        "Restored booking has CANCELLED status"
    );

    check(
        restoreFlight->getAvailableSeats() == 0,
        "Cancelled booking does not change available seats"
    );

    check(
        restoreSystem.getBookingCount() == 2,
        "Both restored bookings remain in the system"
    );
}

    std::cout
        << "\nALL BOOKING SYSTEM TESTS PASSED!\n";
    return 0;
}
