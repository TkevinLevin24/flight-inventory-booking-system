#include <cassert>
#include <iostream>
#include <stdexcept>

#include "Flight.h"

int main() {

    // TEST 1: Initial availability
    Flight flight("AV245", "BOG", "MIA", 2);

    assert(flight.getTotalSeats() == 2);
    assert(flight.getAvailableSeats() == 2);

    std::cout << "TEST 1 PASSED: Initial availability\n";

    // TEST 2: Reserve one seat
    assert(flight.reserveSeat() == true);
    assert(flight.getAvailableSeats() == 1);

    std::cout << "TEST 2 PASSED: Seat reservation\n";

    // TEST 3: Prevent overbooking
    assert(flight.reserveSeat() == true);
    assert(flight.getAvailableSeats() == 0);

    assert(flight.reserveSeat() == false);
    assert(flight.getAvailableSeats() == 0);

    std::cout << "TEST 3 PASSED: Overbooking prevention\n";

    // TEST 4: Release one seat
    assert(flight.releaseSeat() == true);
    assert(flight.getAvailableSeats() == 1);

    std::cout << "TEST 4 PASSED: Seat release\n";

    // TEST 5: Prevent exceeding total capacity
    assert(flight.releaseSeat() == true);
    assert(flight.getAvailableSeats() == 2);

    assert(flight.releaseSeat() == false);
    assert(flight.getAvailableSeats() == 2);

    std::cout << "TEST 5 PASSED: Capacity limit\n";

    // TEST 6: Reject invalid capacity
    bool exceptionThrown = false;

    try {
        Flight invalidFlight("AV999", "BOG", "MIA", 0);
    }
    catch (const std::invalid_argument&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);

    std::cout << "TEST 6 PASSED: Invalid capacity rejected\n";

    std::cout << "\nALL FLIGHT TESTS PASSED!\n";

    return 0;
}