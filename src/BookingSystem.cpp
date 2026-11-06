#include "BookingSystem.h"

#include <iostream>
#include <memory>
#include <tuple>

bool BookingSystem::addFlight(
    const std::string& flightNumber,
    const std::string& origin,
    const std::string& destination,
    int totalSeats
) {
    if (flights.find(flightNumber) != flights.end()) {
        return false;
    }

    if (flightNumber.empty() || origin.empty() ||
        destination.empty() || totalSeats <= 0) {
        return false;
    }

    flights.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(flightNumber),
        std::forward_as_tuple(
            flightNumber,
            origin,
            destination,
            totalSeats
        )
    );

    return true;
}

Flight* BookingSystem::findFlight(
    const std::string& flightNumber
) {
    auto it = flights.find(flightNumber);

    if (it == flights.end()) {
        return nullptr;
    }

    return &it->second;
}

void BookingSystem::displayFlights() const {

    if (flights.empty()) {
        std::cout << "\nNo flights registered.\n";
        return;
    }

    std::cout << "\nREGISTERED FLIGHTS\n";

    for (const auto& entry : flights) {
        entry.second.displayInfo();
    }
}

std::size_t BookingSystem::getFlightCount() const {
    return flights.size();
}

// Passenger management

bool BookingSystem::addPassenger(
    const std::string& passengerId,
    const std::string& name,
    const std::string& email
) {
    if (passengers.find(passengerId) != passengers.end()) {
        return false;
    }

    if (passengerId.empty() ||
        name.empty() ||
        email.empty()) {
        return false;
    }

    passengers.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(passengerId),
        std::forward_as_tuple(
            passengerId,
            name,
            email
        )
    );

    return true;
}

const Passenger* BookingSystem::findPassenger(
    const std::string& passengerId
) const {
    auto it = passengers.find(passengerId);

    if (it == passengers.end()) {
        return nullptr;
    }

    return &it->second;
}

void BookingSystem::displayPassengers() const {

    if (passengers.empty()) {
        std::cout << "\nNo passengers registered.\n";
        return;
    }

    std::cout << "\nREGISTERED PASSENGERS\n";

    for (const auto& entry : passengers) {
        entry.second.displayInfo();
    }
}

std::size_t BookingSystem::getPassengerCount() const {
    return passengers.size();
}

// Booking management

bool BookingSystem::createBooking(
    const std::string& bookingId,
    const std::string& flightNumber,
    const std::string& passengerId
) {
    // Validate booking ID
    if (bookingId.empty()) {
        return false;
    }

    // Prevent duplicate booking IDs
    if (bookings.find(bookingId) != bookings.end()) {
        return false;
    }

    // Find flight
    Flight* flight = findFlight(flightNumber);

    if (flight == nullptr) {
        return false;
    }

    // Find passenger
    const Passenger* passenger = findPassenger(passengerId);

    if (passenger == nullptr) {
        return false;
    }

    // Prevent duplicate active bookings
for (const auto& entry : bookings) {

    const Booking& existingBooking = *entry.second;

    if (
        existingBooking.isConfirmed() &&
        existingBooking.getFlight().getFlightNumber() == flightNumber &&
        existingBooking.getPassenger().getPassengerId() == passengerId
    ) {
        return false;
    }
}

    // Create booking
    auto booking = std::make_unique<Booking>(
        bookingId,
        *flight,
        *passenger
    );

    // Confirm booking
    if (!booking->confirmBooking()) {
        return false;
    }

    // Store confirmed booking
    bookings.emplace(
        bookingId,
        std::move(booking)
    );

    return true;
}

bool BookingSystem::cancelBooking(
    const std::string& bookingId
) {
    auto it = bookings.find(bookingId);

    if (it == bookings.end()) {
        return false;
    }

    return it->second->cancelBooking();
}


bool BookingSystem::rollbackCreatedBooking(
    const std::string& bookingId
) {
    auto it = bookings.find(bookingId);

    if (it == bookings.end()) {
        return false;
    }

    Booking& booking = *it->second;

    // Only confirmed bookings can be rolled back here
    if (!booking.isConfirmed()) {
        return false;
    }

    // Release the reserved seat
    if (!booking.cancelBooking()) {
        return false;
    }

    // Remove the booking from memory
    bookings.erase(it);

    return true;
}


bool BookingSystem::rollbackCancelledBooking(
    const std::string& bookingId
) {
    auto it = bookings.find(bookingId);

    if (it == bookings.end()) {
        return false;
    }

    return it->second->rollbackCancellation();
}



const Booking* BookingSystem::findBooking(
    const std::string& bookingId
) const {
    auto it = bookings.find(bookingId);

    if (it == bookings.end()) {
        return nullptr;
    }

    return it->second.get();
}

void BookingSystem::displayBookings() const {

    if (bookings.empty()) {
        std::cout << "\nNo bookings registered.\n";
        return;
    }

    std::cout << "\nREGISTERED BOOKINGS\n";

    for (const auto& entry : bookings) {
        entry.second->displayInfo();
    }
}

std::size_t BookingSystem::getBookingCount() const {
    return bookings.size();
}
// Execute a callback for each registered flight
void BookingSystem::forEachFlight(
    const std::function<void(const Flight&)>& callback
) const {

    for (const auto& entry : flights) {

        const Flight& flight = entry.second;

        callback(flight);
    }
}
// Execute a callback for each registered passenger
void BookingSystem::forEachPassenger(
    const std::function<void(const Passenger&)>& callback
) const {

    for (const auto& entry : passengers) {

        const Passenger& passenger = entry.second;

        callback(passenger);
    }
}
// Execute a callback for each registered booking
void BookingSystem::forEachBooking(
    const std::function<void(const Booking&)>& callback
) const {

    for (const auto& entry : bookings) {

        const Booking& booking = *entry.second;

        callback(booking);
    }
}
// Restore an existing booking from persistent storage
bool BookingSystem::restoreBooking(
    const std::string& bookingId,
    const std::string& flightNumber,
    const std::string& passengerId,
    BookingStatus status
) {

    // Validate identifiers
    if (
        bookingId.empty() ||
        flightNumber.empty() ||
        passengerId.empty()
    ) {
        return false;
    }

    // Reject duplicate booking IDs
    if (bookings.find(bookingId) != bookings.end()) {
        return false;
    }

    // Find associated flight
    Flight* flight = findFlight(flightNumber);

    if (flight == nullptr) {
        return false;
    }

    // Find associated passenger
    const Passenger* passenger = findPassenger(passengerId);

    if (passenger == nullptr) {
        return false;
    }

    // Validate booking status
    if (
        status != BookingStatus::Pending &&
        status != BookingStatus::Confirmed &&
        status != BookingStatus::Cancelled
    ) {
        return false;
    }

    // Reject another active booking for the same passenger and flight
    if (status == BookingStatus::Confirmed) {

        for (const auto& entry : bookings) {

            const Booking& existingBooking = *entry.second;

            if (
                existingBooking.isConfirmed() &&
                existingBooking.getFlight().getFlightNumber() == flightNumber &&
                existingBooking.getPassenger().getPassengerId() == passengerId
            ) {
                return false;
            }
        }
    }

    // Construct booking
    auto booking = std::make_unique<Booking>(
        bookingId,
        *flight,
        *passenger
    );

    // Restore booking state
    if (status == BookingStatus::Confirmed) {

        if (!booking->confirmBooking()) {
            return false;
        }

    } else if (status == BookingStatus::Cancelled) {

    // Restore historical cancellation without changing seat inventory
    if (!booking->restoreCancelled()) {
        return false;
    }
}

    // Store restored booking
    bookings.emplace(
        bookingId,
        std::move(booking)
    );

    return true;
}