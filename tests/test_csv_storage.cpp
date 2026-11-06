
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "BookingSystem.h"
#include "CsvStorage.h"
#include <filesystem>

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


    // Test: failed CSV replacement must preserve original data.
    {
        namespace fs = std::filesystem;

        const std::string testFilename =
            "test_bookings_protected.csv";

        const std::string temporaryFilename =
            testFilename + ".tmp";

        // Remove leftovers from a previous test run.
        std::error_code cleanupError;
        fs::remove(testFilename, cleanupError);
        fs::remove(temporaryFilename, cleanupError);

        // Create an original CSV file with known content.
        const std::string originalContent =
            "booking_id,flight_number,passenger_id,status\n"
            "B_ORIGINAL,AV900,P900,CANCELLED\n";

        {
            std::ofstream originalFile(
                testFilename,
                std::ios::binary
            );

            check(
                originalFile.is_open(),
                "Protected test CSV opened"
            );

            originalFile << originalContent;
            originalFile.close();

            check(
                !originalFile.fail(),
                "Original test CSV written successfully"
            );
        }

        // Simulate an existing temporary file.
        {
            std::ofstream temporaryFile(temporaryFilename);

            check(
                temporaryFile.is_open(),
                "Blocking temporary file created"
            );

            temporaryFile << "BLOCKED\n";
            temporaryFile.close();

            check(
                !temporaryFile.fail(),
                "Blocking temporary file written"
            );
        }

        // Attempt to save bookings while the temporary
        // filename is already occupied.
        BookingSystem protectedSystem;

        check(
            protectedSystem.addFlight(
                "AV900", "BOG", "MIA", 10
            ),
            "Protected test flight registered"
        );

        check(
            protectedSystem.addPassenger(
                "P900", "Test Passenger", "test@example.com"
            ),
            "Protected test passenger registered"
        );

        check(
            protectedSystem.createBooking(
                "B_NEW", "AV900", "P900"
            ),
            "New booking created in memory"
        );

        const bool saveResult =
            CsvStorage::saveBookings(
                protectedSystem,
                testFilename
            );

        check(
            !saveResult,
            "CSV save rejected because temporary file exists"
        );

        // Read the original file and verify its contents.
        std::ifstream verificationFile(
            testFilename,
            std::ios::binary
        );

        check(
            verificationFile.is_open(),
            "Original CSV remains accessible"
        );


const std::string actualContent(
    (std::istreambuf_iterator<char>(verificationFile)),
    std::istreambuf_iterator<char>()
);

check(
    actualContent == originalContent,
    "Original CSV content preserved after failed save"
);

// Close the file before deleting it.
verificationFile.close();

check(
    !verificationFile.is_open(),
    "Verification file closed successfully"
);

// Remove test files.
std::error_code temporaryRemoveError;
std::error_code originalRemoveError;

const bool temporaryRemoved =
    fs::remove(temporaryFilename, temporaryRemoveError);

const bool originalRemoved =
    fs::remove(testFilename, originalRemoveError);

check(
    temporaryRemoved &&
    originalRemoved &&
    !temporaryRemoveError &&
    !originalRemoveError &&
    !fs::exists(testFilename) &&
    !fs::exists(temporaryFilename),
    "Protected test files removed"
);


        check(
            !fs::exists(testFilename) &&
            !fs::exists(temporaryFilename),
            "Protected test files removed"
        );
    }


    std::cout
        << "\nALL CSV STORAGE TESTS PASSED!\n";

    return 0;
}
