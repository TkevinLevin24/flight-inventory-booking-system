
#include "BookingService.h"
#include "BookingSystem.h"
#include <iostream>
#include <utility>

BookingService::BookingService(
    BookingSystem& system,
    SaveFunction saveFunction,
    bool persistenceHealthy
)
    : system(system),
      saveFunction(std::move(saveFunction)),
      persistenceHealthy(
          persistenceHealthy && static_cast<bool>(this->saveFunction)
      ) {
}

bool BookingService::createBooking(
    const std::string& bookingId,
    const std::string& flightNumber,
    const std::string& passengerId
) {
    if (!persistenceHealthy) {
        return false;
    }

    if (!system.createBooking(
        bookingId,
        flightNumber,
        passengerId
    )) {
        return false;
    }
    
if (!saveFunction(system)) {

    // Undo the booking because persistence failed
    const bool rollbackSuccessful =
        system.rollbackCreatedBooking(bookingId);

    // Disable further modifications after a save failure
    persistenceHealthy = false;

    if (!rollbackSuccessful) {
        std::cerr
            << "CRITICAL: Failed to rollback booking "
            << bookingId << ".\n";
    }

    return false;
}


    return true;
}

bool BookingService::cancelBooking(
    const std::string& bookingId
) {
    if (!persistenceHealthy) {
        return false;
    }

    if (!system.cancelBooking(bookingId)) {
        return false;
    }

    
if (!saveFunction(system)) {

    // Undo cancellation because persistence failed
    const bool rollbackSuccessful =
        system.rollbackCancelledBooking(bookingId);

    persistenceHealthy = false;

    if (!rollbackSuccessful) {
        std::cerr
            << "CRITICAL: Failed to rollback cancellation "
            << bookingId << ".\n";
    }

    return false;
}


    return true;
}

bool BookingService::isPersistenceHealthy() const {
    return persistenceHealthy;
}
