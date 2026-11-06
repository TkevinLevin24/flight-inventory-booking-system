
#include <cstdlib>
#include <iostream>

#include "BookingSystem.h"
#include "BookingService.h"

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << "\n";
        std::exit(EXIT_FAILURE);
    }

    std::cout << "PASSED: " << message << "\n";
}

int main() {

    // TEST 1: Successful booking persistence.
    {
        BookingSystem system;

        check(
            system.addFlight("AV100", "BOG", "MIA", 2),
            "Test flight registered"
        );

        check(
            system.addPassenger(
                "P100", "Test Passenger", "test@example.com"
            ),
            "Test passenger registered"
        );

        int saveCount = 0;

        BookingService service(
            system,
            [&saveCount](const BookingSystem&) {
                ++saveCount;
                return true;
            }
        );

        check(
            service.createBooking("B100", "AV100", "P100"),
            "Booking created and saved successfully"
        );

        check(
            saveCount == 1,
            "Save function called once"
        );

        check(
            service.isPersistenceHealthy(),
            "Persistence remains healthy"
        );

        check(
            system.findBooking("B100") != nullptr,
            "Booking exists in memory"
        );
    }

    // TEST 2: Failed persistence blocks future changes.
    {
        BookingSystem system;

        check(
            system.addFlight("AV200", "BOG", "MDE", 3),
            "Failure test flight registered"
        );

        check(
            system.addPassenger(
                "P200", "First Passenger", "first@example.com"
            ),
            "First failure test passenger registered"
        );

        check(
            system.addPassenger(
                "P201", "Second Passenger", "second@example.com"
            ),
            "Second failure test passenger registered"
        );
        

        int saveCount = 0;

        BookingService service(
            system,
            [&saveCount](const BookingSystem&) {
                ++saveCount;
                return false;
            }
        );

        check(
            !service.createBooking("B200", "AV200", "P200"),
            "Booking operation reports persistence failure"
        );

        
check(
    system.findBooking("B200") == nullptr,
    "Failed booking removed from memory"
);

check(
    system.findFlight("AV200")->getAvailableSeats() == 3,
    "Seat restored after failed persistence"
);


        check(
            !service.isPersistenceHealthy(),
            "Persistence marked unhealthy"
        );

        check(
            saveCount == 1,
            "Failed save attempted once"
        );

        check(
            !service.createBooking("B201", "AV200", "P201"),
            "Second booking blocked after persistence failure"
        );

        check(
            system.findBooking("B201") == nullptr,
            "Blocked booking was not inserted"
        );

        check(
            !service.cancelBooking("B200"),
            "Cancellation blocked after persistence failure"
        );

        check(
            saveCount == 1,
            "No additional save attempts after failure"
        );
    }

    // TEST 3: Successful cancellation persistence.
    {
        BookingSystem system;

        check(
            system.addFlight("AV300", "BOG", "SCL", 1),
            "Cancellation test flight registered"
        );

        check(
            system.addPassenger(
                "P300", "Third Passenger", "third@example.com"
            ),
            "Cancellation test passenger registered"
        );

        int saveCount = 0;

        BookingService service(
            system,
            [&saveCount](const BookingSystem&) {
                ++saveCount;
                return true;
            }
        );

        check(
            service.createBooking("B300", "AV300", "P300"),
            "Cancellation test booking created"
        );

        check(
            system.findFlight("AV300")->getAvailableSeats() == 0,
            "Seat reserved successfully"
        );

        check(
            service.cancelBooking("B300"),
            "Booking cancelled and saved successfully"
        );

        check(
            system.findFlight("AV300")->getAvailableSeats() == 1,
            "Seat released after cancellation"
        );

        check(
            saveCount == 2,
            "Save called for creation and cancellation"
        );

        check(
            service.isPersistenceHealthy(),
            "Persistence healthy after cancellation"
        );
    }

    
    // TEST 4: Failed cancellation persistence restores booking.
    {
        BookingSystem system;

        check(
            system.addFlight("AV400", "BOG", "CTG", 1),
            "Rollback cancellation flight registered"
        );

        check(
            system.addPassenger(
                "P400", "Fourth Passenger", "fourth@example.com"
            ),
            "Rollback cancellation passenger registered"
        );

        int saveCount = 0;
        bool shouldSave = true;

        BookingService service(
            system,
            [&saveCount, &shouldSave](const BookingSystem&) {
                ++saveCount;
                return shouldSave;
            }
        );

        check(
            service.createBooking("B400", "AV400", "P400"),
            "Initial booking created successfully"
        );

        check(
            system.findFlight("AV400")->getAvailableSeats() == 0,
            "Seat occupied before cancellation"
        );

        // Simulate a storage failure.
        shouldSave = false;

        check(
            !service.cancelBooking("B400"),
            "Cancellation reports persistence failure"
        );

        const Booking* booking = system.findBooking("B400");

        check(
            booking != nullptr && booking->isConfirmed(),
            "Booking restored to CONFIRMED"
        );

        check(
            system.findFlight("AV400")->getAvailableSeats() == 0,
            "Seat reserved again after rollback"
        );

        check(
            !service.isPersistenceHealthy(),
            "Persistence disabled after cancellation failure"
        );

        check(
            saveCount == 2,
            "Save attempted for creation and cancellation"
        );

        check(
            !service.cancelBooking("B400"),
            "Further cancellations blocked"
        );

        check(
            saveCount == 2,
            "No additional save attempts after failure"
        );
    }


    std::cout
        << "\nALL BOOKING SERVICE TESTS PASSED!\n";

    return EXIT_SUCCESS;
}
