#include "Booking.h"

#include <iostream>
#include <stdexcept>

// Constructor
Booking::Booking(
    const std::string& bookingId,
    Flight& flight,
    const Passenger& passenger
)
    : bookingId(bookingId),
      flight(flight),
      passenger(passenger),
      status(BookingStatus::Pending)
{
    if (bookingId.empty()) {
        throw std::invalid_argument(
            "Booking ID cannot be empty"
        );
    }
}

// Confirm booking
bool Booking::confirmBooking() {

    // Only pending bookings can be confirmed
    if (status != BookingStatus::Pending) {
        return false;
    }

    // Reserve a seat
    if (!flight.reserveSeat()) {
        return false;
    }

    status = BookingStatus::Confirmed;

    return true;
}

// Cancel booking
bool Booking::cancelBooking() {

    // Only confirmed bookings can be cancelled
    if (status != BookingStatus::Confirmed) {
        return false;
    }

    // Release the reserved seat
    if (!flight.releaseSeat()) {
        return false;
    }

    status = BookingStatus::Cancelled;

    return true;
}

// Check confirmation status
bool Booking::isConfirmed() const {
    return status == BookingStatus::Confirmed;
}

// Get booking status
BookingStatus Booking::getStatus() const {
    return status;
}

// Get booking ID
const std::string& Booking::getBookingId() const {
    return bookingId;
}

// Get associated flight
const Flight& Booking::getFlight() const {
    return flight;
}

// Get associated passenger
const Passenger& Booking::getPassenger() const {
    return passenger;
}

// Display booking information
void Booking::displayInfo() const {

    std::cout << "\nBooking ID: "
              << bookingId << "\n";

    std::cout << "Passenger: "
              << passenger.getName() << "\n";

    std::cout << "Flight: "
              << flight.getFlightNumber() << "\n";

    std::cout << "Route: "
              << flight.getOrigin()
              << " -> "
              << flight.getDestination()
              << "\n";

    std::cout << "Status: ";

    switch (status) {

        case BookingStatus::Pending:
            std::cout << "PENDING";
            break;

        case BookingStatus::Confirmed:
            std::cout << "CONFIRMED";
            break;

        case BookingStatus::Cancelled:
            std::cout << "CANCELLED";
            break;
    }

    std::cout << "\n";
}
// Restore a previously cancelled booking without changing seat inventory
bool Booking::restoreCancelled() {

    if (status != BookingStatus::Pending) {
        return false;
    }

    status = BookingStatus::Cancelled;

    return true;
}