#ifndef BOOKING_H
#define BOOKING_H

#include <string>

#include "Flight.h"
#include "Passenger.h"

// Possible booking states
enum class BookingStatus {
    Pending,
    Confirmed,
    Cancelled
};

class Booking {

private:
    std::string bookingId;

    Flight& flight;

    const Passenger& passenger;

    BookingStatus status;

public:
    Booking(
        const std::string& bookingId,
        Flight& flight,
        const Passenger& passenger
    );

    bool confirmBooking();

    bool restoreCancelled();
    // Restore a cancelled booking after a failed save
bool rollbackCancellation();

    bool cancelBooking();

    bool isConfirmed() const;

    BookingStatus getStatus() const;

    const std::string& getBookingId() const;

    const Flight& getFlight() const;

    const Passenger& getPassenger() const;

    void displayInfo() const;
};

#endif