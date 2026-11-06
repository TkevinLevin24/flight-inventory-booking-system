#ifndef BOOKING_SYSTEM_H
#include <functional>
#define BOOKING_SYSTEM_H

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

#include "Flight.h"
#include "Passenger.h"
#include "Booking.h"

class BookingSystem {

private:
    std::unordered_map<std::string, Flight> flights;

    std::unordered_map<std::string, Passenger> passengers;

    std::unordered_map<
        std::string,
        std::unique_ptr<Booking>
    > bookings;

public:

    BookingSystem() = default;

    BookingSystem(const BookingSystem&) = delete;

    BookingSystem& operator=(
        const BookingSystem&
    ) = delete;

    BookingSystem(BookingSystem&&) = delete;

    BookingSystem& operator=(
        BookingSystem&&
    ) = delete;
    // Flight management
    bool addFlight(
        const std::string& flightNumber,
        const std::string& origin,
        const std::string& destination,
        int totalSeats
    );

    Flight* findFlight(const std::string& flightNumber);

    void displayFlights() const;
    void forEachFlight(
    const std::function<void(const Flight&)>& callback
) const;
void forEachPassenger(
    const std::function<void(const Passenger&)>& callback
) const;
void forEachBooking(
    const std::function<void(const Booking&)>& callback
) const;

    std::size_t getFlightCount() const;

    // Passenger management
    bool addPassenger(
        const std::string& passengerId,
        const std::string& name,
        const std::string& email
    );

    const Passenger* findPassenger(
        const std::string& passengerId
    ) const;

    void displayPassengers() const;

    std::size_t getPassengerCount() const;

    // Booking management
    bool createBooking(
        const std::string& bookingId,
        const std::string& flightNumber,
        const std::string& passengerId
    );

    bool restoreBooking(
    const std::string& bookingId,
    const std::string& flightNumber,
    const std::string& passengerId,
    BookingStatus status
);

    bool cancelBooking(const std::string& bookingId);


    // Undo a newly created confirmed booking
    bool rollbackCreatedBooking(
        const std::string& bookingId
    );

    // Undo a cancellation after a failed save
bool rollbackCancelledBooking(
    const std::string& bookingId
);


    const Booking* findBooking(
        const std::string& bookingId
    ) const;

    void displayBookings() const;

    std::size_t getBookingCount() const;
};

#endif