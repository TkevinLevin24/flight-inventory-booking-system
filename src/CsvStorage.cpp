#include "CsvStorage.h"
#include "BookingSystem.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <limits>
#include <stdexcept>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <filesystem>
#include <system_error>


// Save registered flights to a CSV file
bool CsvStorage::saveFlights(
    const BookingSystem& system,
    const std::string& filename
) {

    std::ofstream file(filename);

    if (!file.is_open()) {

        std::cerr
            << "ERROR: Could not open file: "
            << filename
            << "\n";

        return false;
    }

    // Write CSV header
    file << "flight_number,origin,destination,total_seats\n";

    // Write each flight
    system.forEachFlight([&file](const Flight& flight) {

        file << flight.getFlightNumber() << ","
             << flight.getOrigin() << ","
             << flight.getDestination() << ","
             << flight.getTotalSeats() << "\n";
    });

    file.flush();

    if (!file.good()) {

        std::cerr
            << "ERROR: Failed to write CSV file.\n";

        return false;
    }

    return true;
}
// Load registered flights from a CSV file
bool CsvStorage::loadFlights(
    BookingSystem& system,
    const std::string& filename
) {

    std::ifstream file(filename);

    if (!file.is_open()) {

        std::cerr
            << "WARNING: Could not open file: "
            << filename
            << "\n";

        return false;
    }

    std::string line;

    // Read and validate CSV header
    if (!std::getline(file, line)) {

        std::cerr
            << "ERROR: CSV file is empty.\n";

        return false;
    }

    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    if (line != "flight_number,origin,destination,total_seats") {

        std::cerr
            << "ERROR: Invalid flights CSV header.\n";

        return false;
    }

    std::size_t loadedFlights = 0;
    std::size_t skippedFlights = 0;

    // Read each flight record
    while (std::getline(file, line)) {

        if (line.empty()) {
            continue;
        }

        std::stringstream stream(line);

        std::string flightNumber;
        std::string origin;
        std::string destination;
        std::string seatsText;
        std::string extraField;

        // Read the four expected columns
        if (
            !std::getline(stream, flightNumber, ',') ||
            !std::getline(stream, origin, ',') ||
            !std::getline(stream, destination, ',') ||
            !std::getline(stream, seatsText, ',')
        ) {

            ++skippedFlights;
            continue;
        }

        // Reject unexpected additional columns
        if (std::getline(stream, extraField, ',')) {

            ++skippedFlights;
            continue;
        }

        // Validate required fields
        if (
            flightNumber.empty() ||
            origin.empty() ||
            destination.empty() ||
            seatsText.empty()
        ) {

            ++skippedFlights;
            continue;
        }

        // Validate seat capacity
        int totalSeats = 0;

        try {

            std::size_t processed = 0;

            long long seats = std::stoll(
                seatsText,
                &processed
            );

            if (
                processed != seatsText.length() ||
                seats <= 0 ||
                seats > std::numeric_limits<int>::max()
            ) {

                ++skippedFlights;
                continue;
            }

            totalSeats = static_cast<int>(seats);

        } catch (const std::exception&) {

            ++skippedFlights;
            continue;
        }

        // Register flight in memory
        if (system.addFlight(
                flightNumber,
                origin,
                destination,
                totalSeats
            )) {

            ++loadedFlights;

        } else {

            ++skippedFlights;
        }
    }

    if (file.bad()) {

        std::cerr
            << "ERROR: Failed while reading CSV file.\n";

        return false;
    }

    std::cout
        << "\nLoaded "
        << loadedFlights
        << " flights from CSV.\n";

    if (skippedFlights > 0) {

        std::cout
            << "WARNING: Skipped "
            << skippedFlights
            << " invalid or duplicate records.\n";
    }

    return skippedFlights == 0;
}
// Save registered passengers to a CSV file
bool CsvStorage::savePassengers(
    const BookingSystem& system,
    const std::string& filename
) {

    std::ofstream file(filename);

    if (!file.is_open()) {

        std::cerr
            << "ERROR: Could not open file: "
            << filename
            << "\n";

        return false;
    }

    // Write CSV header
    file << "passenger_id,name,email\n";

    // Write each passenger
    system.forEachPassenger([&file](const Passenger& passenger) {

        file << passenger.getPassengerId() << ","
             << passenger.getName() << ","
             << passenger.getEmail() << "\n";
    });

    file.flush();

    if (!file.good()) {

        std::cerr
            << "ERROR: Failed to write passengers CSV file.\n";

        return false;
    }

    return true;
}
// Load registered passengers from a CSV file
bool CsvStorage::loadPassengers(
    BookingSystem& system,
    const std::string& filename
) {

    std::ifstream file(filename);

    if (!file.is_open()) {

        std::cerr
            << "WARNING: Could not open file: "
            << filename
            << "\n";

        return false;
    }

    std::string line;

    // Read and validate CSV header
    if (!std::getline(file, line)) {

        std::cerr
            << "ERROR: Passengers CSV file is empty.\n";

        return false;
    }

    // Remove Windows carriage return if present
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    if (line != "passenger_id,name,email") {

        std::cerr
            << "ERROR: Invalid passengers CSV header.\n";

        return false;
    }

    std::size_t loadedPassengers = 0;
    std::size_t skippedPassengers = 0;

    // Read passenger records
    while (std::getline(file, line)) {

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) {
            continue;
        }

        std::stringstream stream(line);

        std::string passengerId;
        std::string name;
        std::string email;
        std::string extraField;

        // Read the three expected columns
        if (
            !std::getline(stream, passengerId, ',') ||
            !std::getline(stream, name, ',') ||
            !std::getline(stream, email, ',')
        ) {

            ++skippedPassengers;
            continue;
        }

        // Reject additional columns
        if (std::getline(stream, extraField, ',')) {

            ++skippedPassengers;
            continue;
        }

        // Validate required fields
        if (
            passengerId.empty() ||
            name.empty() ||
            email.empty()
        ) {

            ++skippedPassengers;
            continue;
        }

        // Register passenger in memory
        if (system.addPassenger(
                passengerId,
                name,
                email
            )) {

            ++loadedPassengers;

        } else {

            ++skippedPassengers;
        }
    }

    if (file.bad()) {

        std::cerr
            << "ERROR: Failed while reading passengers CSV file.\n";

        return false;
    }

    std::cout
        << "\nLoaded "
        << loadedPassengers
        << " passengers from CSV.\n";

    if (skippedPassengers > 0) {

        std::cout
            << "WARNING: Skipped "
            << skippedPassengers
            << " invalid or duplicate passenger records.\n";
    }

    return skippedPassengers == 0;
}

bool CsvStorage::saveBookings(
    const BookingSystem& system,
    const std::string& filename
) {
    namespace fs = std::filesystem;

    const fs::path targetPath(filename);
    const fs::path temporaryPath = targetPath.string() + ".tmp";

    // Avoid overwriting an existing temporary file.
    std::error_code error;

    if (fs::exists(temporaryPath, error)) {
        std::cerr
            << "ERROR: Temporary bookings file already exists.\n";
        return false;
    }

    if (error) {
        std::cerr
            << "ERROR: Could not check temporary bookings file.\n";
        return false;
    }

    // Create the temporary CSV file.
    std::ofstream file(temporaryPath, std::ios::out | std::ios::trunc);

    if (!file.is_open()) {
        std::cerr
            << "ERROR: Could not create temporary bookings file.\n";
        return false;
    }

    file << "booking_id,flight_number,passenger_id,status\n";

    // Write all bookings to the temporary file.
    system.forEachBooking([&file](const Booking& booking) {
        std::string status;

        switch (booking.getStatus()) {
            case BookingStatus::Pending:
                status = "PENDING";
                break;

            case BookingStatus::Confirmed:
                status = "CONFIRMED";
                break;

            case BookingStatus::Cancelled:
                status = "CANCELLED";
                break;
        }

        file << booking.getBookingId() << ","
             << booking.getFlight().getFlightNumber() << ","
             << booking.getPassenger().getPassengerId() << ","
             << status << "\n";
    });

    // Flush buffered data before closing.
    file.flush();

    if (!file.good()) {
        std::cerr
            << "ERROR: Failed to write temporary bookings file.\n";

        file.close();
        fs::remove(temporaryPath, error);
        return false;
    }

    file.close();

    if (file.fail()) {
        std::cerr
            << "ERROR: Failed to close temporary bookings file.\n";

        fs::remove(temporaryPath, error);
        return false;
    }

    // Replace the original CSV only after successful writing.
    fs::rename(temporaryPath, targetPath, error);

    if (error) {
        std::cerr
            << "ERROR: Could not replace bookings CSV file: "
            << error.message() << "\n";

        return false;
    }

    return true;
}


// Load bookings from a CSV file
bool CsvStorage::loadBookings(
    BookingSystem& system,
    const std::string& filename
) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open bookings file: "
                  << filename << "\n";
        return false;
    }

    std::string line;

    // Validate CSV header
    if (!std::getline(file, line)) {
        std::cerr << "ERROR: Bookings CSV file is empty.\n";
        return false;
    }

    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    if (line != "booking_id,flight_number,passenger_id,status") {
        std::cerr << "ERROR: Invalid bookings CSV header.\n";
        return false;
    }

    // Temporary representation of a booking
    struct BookingRecord {
        std::string bookingId;
        std::string flightNumber;
        std::string passengerId;
        BookingStatus status;
    };

    std::vector<BookingRecord> records;
    std::unordered_set<std::string> bookingIds;
    std::unordered_set<std::string> confirmedPassengerFlights;
    std::unordered_map<std::string, int> confirmedSeats;

    // Check bookings already registered in the system
    system.forEachBooking([&](const Booking& booking) {
        bookingIds.insert(booking.getBookingId());

        if (booking.isConfirmed()) {
            const std::string flightNumber =
                booking.getFlight().getFlightNumber();

            const std::string passengerId =
                booking.getPassenger().getPassengerId();

            const std::string key =
                std::to_string(flightNumber.size()) + ":" +
                flightNumber + passengerId;

            confirmedPassengerFlights.insert(key);
        }
    });

    // PHASE 1: Read and validate all CSV records
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) {
            continue;
        }

        std::stringstream stream(line);

        std::string bookingId;
        std::string flightNumber;
        std::string passengerId;
        std::string statusText;
        std::string extraField;

        // Read the four required fields
        if (!std::getline(stream, bookingId, ',') ||
            !std::getline(stream, flightNumber, ',') ||
            !std::getline(stream, passengerId, ',') ||
            !std::getline(stream, statusText, ',') ||
            std::getline(stream, extraField, ',')) {

            std::cerr << "ERROR: Invalid booking CSV record.\n";
            return false;
        }

        // Reject empty fields
        if (bookingId.empty() ||
            flightNumber.empty() ||
            passengerId.empty() ||
            statusText.empty()) {

            std::cerr << "ERROR: Empty booking CSV field.\n";
            return false;
        }

        // Convert text into BookingStatus
        BookingStatus status;

        if (statusText == "PENDING") {
            status = BookingStatus::Pending;

        } else if (statusText == "CONFIRMED") {
            status = BookingStatus::Confirmed;

        } else if (statusText == "CANCELLED") {
            status = BookingStatus::Cancelled;

        } else {
            std::cerr << "ERROR: Invalid booking status.\n";
            return false;
        }

        // Validate flight and passenger references
        const Flight* flight = system.findFlight(flightNumber);
        const Passenger* passenger = system.findPassenger(passengerId);

        if (flight == nullptr || passenger == nullptr) {
            std::cerr << "ERROR: Booking references missing data.\n";
            return false;
        }

        // Reject duplicate booking IDs
        if (!bookingIds.insert(bookingId).second) {
            std::cerr << "ERROR: Duplicate booking ID.\n";
            return false;
        }

        // Validate confirmed bookings
        if (status == BookingStatus::Confirmed) {
            const std::string key =
                std::to_string(flightNumber.size()) + ":" +
                flightNumber + passengerId;

            if (!confirmedPassengerFlights.insert(key).second) {
                std::cerr
                    << "ERROR: Duplicate confirmed passenger flight.\n";
                return false;
            }

            ++confirmedSeats[flightNumber];

            if (confirmedSeats[flightNumber] >
                flight->getAvailableSeats()) {

                std::cerr
                    << "ERROR: Not enough seats to restore bookings.\n";
                return false;
            }
        }

        // Store validated record temporarily
        records.push_back({
            bookingId,
            flightNumber,
            passengerId,
            status
        });
    }

    if (file.bad()) {
        std::cerr << "ERROR: Failed while reading bookings CSV.\n";
        return false;
    }

    // PHASE 2: Restore bookings after successful validation
    for (const BookingRecord& record : records) {
        if (!system.restoreBooking(
                record.bookingId,
                record.flightNumber,
                record.passengerId,
                record.status
            )) {

            std::cerr
                << "ERROR: Failed to restore validated booking.\n";
            return false;
        }
    }

    std::cout << "\nLoaded "
              << records.size()
              << " bookings from CSV.\n";

    return true;
}
