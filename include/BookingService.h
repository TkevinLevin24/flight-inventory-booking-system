
#ifndef BOOKING_SERVICE_H
#define BOOKING_SERVICE_H

#include <functional>
#include <string>

class BookingSystem;

class BookingService {
public:
    using SaveFunction =
        std::function<bool(const BookingSystem&)>;

    BookingService(
        BookingSystem& system,
        SaveFunction saveFunction,
        bool persistenceHealthy = true
    );

    bool createBooking(
        const std::string& bookingId,
        const std::string& flightNumber,
        const std::string& passengerId
    );

    bool cancelBooking(const std::string& bookingId);

    bool isPersistenceHealthy() const;

private:
    BookingSystem& system;
    SaveFunction saveFunction;
    bool persistenceHealthy;
};

#endif
    