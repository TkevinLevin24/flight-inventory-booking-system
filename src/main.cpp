#include <iostream>
#include <string>
#include <stdexcept>
#include <cctype>
#include <limits>
#include "BookingSystem.h"
#include "CsvStorage.h"

// Display main menu
void displayMenu() {

    std::cout << "\n========================================\n";
    std::cout << "       FLIGHT BOOKING SYSTEM\n";
    std::cout << "========================================\n";

    std::cout << "1. Register flight\n";
    std::cout << "2. Register passenger\n";
    std::cout << "3. Create booking\n";
    std::cout << "4. Cancel booking\n";
    std::cout << "5. Display flights\n";
    std::cout << "6. Display passengers\n";
    std::cout << "7. Display bookings\n";
    std::cout << "8. Search booking\n";
    std::cout << "9. System statistics\n";
    std::cout << "0. Exit\n";

    std::cout << "\nSelect an option: ";
}

// Read a complete line
std::string readInput(const std::string& message) {

    std::string input;

    std::cout << message;

    if (!std::getline(std::cin, input)) {

        throw std::runtime_error(
            "Input stream closed."
        );
    }

    return input;
}
// Check whether the user wants to cancel an operation
bool isCancelled(const std::string& input) {
    return input == "0";
}

// Validate positive integer input
bool parsePositiveInteger(
    const std::string& input,
    int& result
) {

    if (input.empty()) {
        return false;
    }

    // Check that every character is a digit
    for (unsigned char character : input) {

        if (!std::isdigit(character)) {
            return false;
        }
    }

    try {

        std::size_t processedCharacters = 0;

        long long value = std::stoll(
            input,
            &processedCharacters
        );

        if (
            processedCharacters != input.length() ||
            value <= 0 ||
            value > std::numeric_limits<int>::max()
        ) {
            return false;
        }

        result = static_cast<int>(value);

        return true;

    } catch (const std::exception&) {

        return false;
    }
}

// Register flight
void registerFlight(BookingSystem& system) {

    std::cout << "\nEnter 0 in any field to return to the menu.\n\n";

    std::string flightNumber = readInput("Flight number: ");

    if (isCancelled(flightNumber)) {
        std::cout << "\nOperation cancelled. Returning to main menu.\n";
        return;
    }

    std::string origin = readInput("Origin: ");

    if (isCancelled(origin)) {
        std::cout << "\nOperation cancelled. Returning to main menu.\n";
        return;
    }

    std::string destination = readInput("Destination: ");

    if (isCancelled(destination)) {
        std::cout << "\nOperation cancelled. Returning to main menu.\n";
        return;
    }

    std::string seatsInput = readInput("Total seats: ");

    if (isCancelled(seatsInput)) {
        std::cout << "\nOperation cancelled. Returning to main menu.\n";
        return;
    }

    // Validate required fields
    if (
        flightNumber.empty() ||
        origin.empty() ||
        destination.empty()
    ) {
        std::cout << "\nERROR: All flight fields are required.\n";
        return;
    }

    // Validate seat capacity
    int totalSeats = 0;

    if (!parsePositiveInteger(seatsInput, totalSeats)) {
        std::cout
            << "\nERROR: Seat capacity must be "
            << "a positive whole number.\n";
        return;
    }

    // Check duplicate flight
    if (system.findFlight(flightNumber) != nullptr) {
        std::cout << "\nERROR: Flight "
                  << flightNumber
                  << " already exists.\n";
        return;
    }

   // Register flight
if (system.addFlight(
        flightNumber,
        origin,
        destination,
        totalSeats
    )) {

    std::cout
        << "\nFlight registered successfully!\n";

    // Save registered flights to CSV
    if (CsvStorage::saveFlights(system, "data/flights.csv")) {

        std::cout
            << "Flight inventory saved to CSV successfully!\n";

    } else {

        std::cout
            << "\nWARNING: Flight registered in memory, "
            << "but could not be saved to CSV.\n";
    }

} else {

    std::cout
        << "\nERROR: Failed to register flight.\n";
}
}

// Register passenger
void registerPassenger(BookingSystem& system) {

    std::cout << "\nEnter 0 in any field to return to the menu.\n\n";

    std::string passengerId = readInput("Passenger ID: ");

    if (isCancelled(passengerId)) {
        std::cout << "\nOperation cancelled. Returning to main menu.\n";
        return;
    }

    std::string name = readInput("Passenger name: ");

    if (isCancelled(name)) {
        std::cout << "\nOperation cancelled. Returning to main menu.\n";
        return;
    }

    std::string email = readInput("Passenger email: ");

    if (isCancelled(email)) {
        std::cout << "\nOperation cancelled. Returning to main menu.\n";
        return;
    }

    // Validate required fields
    if (
        passengerId.empty() ||
        name.empty() ||
        email.empty()
    ) {
        std::cout << "\nERROR: All passenger fields are required.\n";
        return;
    }

    // Check duplicate passenger
    if (system.addPassenger(
        passengerId,
        name,
        email
    )) {

    std::cout
        << "\nPassenger registered successfully!\n";

    // Save registered passengers to CSV
    if (CsvStorage::savePassengers(
            system,
            "data/passengers.csv"
        )) {

        std::cout
            << "Passenger data saved to CSV successfully!\n";

    } else {

        std::cout
            << "\nWARNING: Passenger registered in memory, "
            << "but could not be saved to CSV.\n";
    }

} else {

    std::cout
        << "\nERROR: Failed to register passenger.\n";
}
}

// Create booking
// Create booking
void createBooking(BookingSystem& system) {

    std::cout << "\nEnter 0 in any field to return to the menu.\n\n";

    std::string bookingId = readInput("Booking ID: ");

    if (isCancelled(bookingId)) {
        std::cout << "\nOperation cancelled. Returning to main menu.\n";
        return;
    }

    std::string flightNumber = readInput("Flight number: ");

    if (isCancelled(flightNumber)) {
        std::cout << "\nOperation cancelled. Returning to main menu.\n";
        return;
    }

    std::string passengerId = readInput("Passenger ID: ");

    if (isCancelled(passengerId)) {
        std::cout << "\nOperation cancelled. Returning to main menu.\n";
        return;
    }

    // Validate empty fields
    if (
        bookingId.empty() ||
        flightNumber.empty() ||
        passengerId.empty()
    ) {
        std::cout << "\nERROR: All fields are required.\n";
        return;
    }

    // Validate duplicate booking ID
    if (system.findBooking(bookingId) != nullptr) {
        std::cout << "\nERROR: Booking ID already exists.\n";
        return;
    }

    // Validate flight
    Flight* flight = system.findFlight(flightNumber);

    if (flight == nullptr) {
        std::cout << "\nERROR: Flight "
                  << flightNumber
                  << " does not exist.\n";
        return;
    }

    // Validate passenger
    const Passenger* passenger =
        system.findPassenger(passengerId);

    if (passenger == nullptr) {
        std::cout << "\nERROR: Passenger "
                  << passengerId
                  << " does not exist.\n";
        return;
    }

    // Validate duplicate active booking
    // This check will be handled by BookingSystem.
    // The core system remains responsible for business rules.

    // Validate seat availability
    if (flight->getAvailableSeats() <= 0) {
        std::cout << "\nERROR: Flight "
                  << flightNumber
                  << " has no available seats.\n";
        return;
    }

    // Attempt booking
    
if (system.createBooking(bookingId, flightNumber, passengerId)) {
    std::cout << "\nBooking created successfully.\n";

    if (CsvStorage::saveBookings(system, "data/bookings.csv")) {
        std::cout << "Booking saved successfully to CSV.\n";
    } else {
        std::cerr
            << "ERROR: Booking was created in memory, "
            << "but could not be saved to CSV.\n";
    }

} else {
    std::cout << "\nERROR: Could not create booking.\n";
}

}

// Cancel booking
void cancelBooking(BookingSystem& system) {

    std::cout << "\nEnter 0 to return to the menu.\n\n";

    std::string bookingId = readInput("Booking ID to cancel: ");

    // Check cancellation
    if (isCancelled(bookingId)) {

        std::cout
            << "\nOperation cancelled. Returning to main menu.\n";

        return;
    }

    // Validate empty input
    if (bookingId.empty()) {

        std::cout
            << "\nERROR: Booking ID is required.\n";

        return;
    }

    // Find booking
    const Booking* booking = system.findBooking(bookingId);

    if (booking == nullptr) {

        std::cout
            << "\nERROR: Booking "
            << bookingId
            << " does not exist.\n";

        return;
    }

    // Attempt cancellation
   
if (system.cancelBooking(bookingId)) {
    std::cout << "\nBooking cancelled successfully.\n";

    if (CsvStorage::saveBookings(system, "data/bookings.csv")) {
        std::cout << "Booking cancellation saved to CSV.\n";
    } else {
        std::cerr
            << "ERROR: Booking was cancelled in memory, "
            << "but could not be saved to CSV.\n";
    }

} else {
    std::cout << "\nERROR: Could not cancel booking.\n";
}

}

// Search booking
void searchBooking(const BookingSystem& system) {

    std::cout << "\nEnter 0 to return to the menu.\n\n";

    std::string bookingId = readInput("Booking ID to search: ");

    // Check cancellation
    if (isCancelled(bookingId)) {

        std::cout
            << "\nOperation cancelled. Returning to main menu.\n";

        return;
    }

    // Validate empty input
    if (bookingId.empty()) {

        std::cout
            << "\nERROR: Booking ID is required.\n";

        return;
    }

    // Search booking
    const Booking* booking = system.findBooking(bookingId);

    if (booking != nullptr) {

        std::cout << "\nBooking found:\n";

        booking->displayInfo();

    } else {

        std::cout
            << "\nERROR: Booking "
            << bookingId
            << " does not exist.\n";
    }
}

// Display system statistics
void displayStatistics(const BookingSystem& system) {

    std::cout << "\n========================================\n";
    std::cout << "          SYSTEM STATISTICS\n";
    std::cout << "========================================\n";

    std::cout << "Registered flights: "
              << system.getFlightCount() << "\n";

    std::cout << "Registered passengers: "
              << system.getPassengerCount() << "\n";

    std::cout << "Booking records: "
              << system.getBookingCount() << "\n";
}

int main() {

    BookingSystem system;

    // Load saved flights at startup
    const bool flightsLoadedSuccessfully =
        CsvStorage::loadFlights(system, "data/flights.csv");

    if (flightsLoadedSuccessfully) {

        std::cout
            << "Flight inventory initialized successfully.\n";

    } else {

        std::cout
            << "WARNING: Flight inventory could not be fully loaded.\n";

        std::cout
            << "Flight registration is disabled to protect CSV data.\n";
    }

    // Load saved passengers at startup
const bool passengersLoadedSuccessfully =
    CsvStorage::loadPassengers(system, "data/passengers.csv");

if (passengersLoadedSuccessfully) {

    std::cout
        << "Passenger database initialized successfully.\n";

} else {

    std::cout
        << "WARNING: Passenger database could not be fully loaded.\n";

    std::cout
        << "Passenger registration is disabled to protect CSV data.\n";
}


    // Load saved bookings after flights and passengers
    bool bookingsLoadedSuccessfully = false;

    if (flightsLoadedSuccessfully && passengersLoadedSuccessfully) {
        bookingsLoadedSuccessfully =
            CsvStorage::loadBookings(system, "data/bookings.csv");

        if (bookingsLoadedSuccessfully) {
            std::cout
                << "Booking database initialized successfully.\n";
        } else {
            std::cout
                << "WARNING: Booking database could not be loaded.\n";
            std::cout
                << "Booking changes are disabled to protect saved data.\n";
        }
    } else {
        std::cout
            << "WARNING: Bookings cannot be loaded because "
            << "flights or passengers failed to load.\n";
    }


    bool running = true;

    try {

        while (running) {

            displayMenu();

            std::string option = readInput("");

            if (option == "0") {

                running = false;

                std::cout
                    << "\nExiting system. Goodbye!\n";

           } else if (option == "1") {

    if (flightsLoadedSuccessfully) {

        registerFlight(system);

    } else {

        std::cout
            << "\nERROR: Flight registration is disabled "
            << "because the CSV inventory was not loaded correctly.\n";
    }

            } else if (option == "2") {

    if (passengersLoadedSuccessfully) {

        registerPassenger(system);

    } else {

        std::cout
            << "\nERROR: Passenger registration is disabled "
            << "because the CSV database was not loaded correctly.\n";
    }

            } else if (option == "3") {
                if (bookingsLoadedSuccessfully) {
                    createBooking(system);
                    } else {
                    std::cout
                        << "\nERROR: Booking creation is disabled "
                        << "because saved bookings were not loaded.\n";
                }
            } else if (option == "4") {
                if (bookingsLoadedSuccessfully) {
                    cancelBooking(system);
                     } else {
                    std::cout
                        << "\nERROR: Booking cancellation is disabled "
                        << "because saved bookings were not loaded.\n";

            }
         } else if (option == "5") {

                system.displayFlights();

            } else if (option == "6") {

                system.displayPassengers();

            } else if (option == "7") {

                system.displayBookings();

            } else if (option == "8") {

                searchBooking(system);

            } else if (option == "9") {

                displayStatistics(system);

            } else {

                std::cout
                    << "\nInvalid option. Try again.\n";
            }
        }

    } catch (const std::runtime_error& error) {

        std::cerr
            << "\nInput closed. Exiting system safely.\n";

        return 0;
    }

    return 0;
}