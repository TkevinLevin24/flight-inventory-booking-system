#include "Flight.h"

#include <iostream>
#include <stdexcept>

Flight::Flight(
    const std::string& flightNumber,
    const std::string& origin,
    const std::string& destination,
    int totalSeats
)
    : flightNumber(flightNumber),
      origin(origin),
      destination(destination),
      totalSeats(totalSeats),
      availableSeats(totalSeats)
{
    if (totalSeats <= 0) {
        throw std::invalid_argument(
            "Total seats must be greater than zero."
        );
    }
}

// Getters

std::string Flight::getFlightNumber() const {
    return flightNumber;
}

std::string Flight::getOrigin() const {
    return origin;
}

std::string Flight::getDestination() const {
    return destination;
}

int Flight::getTotalSeats() const {
    return totalSeats;
}

int Flight::getAvailableSeats() const {
    return availableSeats;
}

// Seat management

bool Flight::reserveSeat() {

    if (availableSeats <= 0) {
        return false;
    }

    availableSeats--;

    return true;
}

bool Flight::releaseSeat() {

    if (availableSeats >= totalSeats) {
        return false;
    }

    availableSeats++;

    return true;
}

// Display information

void Flight::displayInfo() const {

    std::cout << "\nFlight: " << flightNumber << "\n";

    std::cout << "Route: "
              << origin
              << " -> "
              << destination
              << "\n";

    std::cout << "Total seats: "
              << totalSeats
              << "\n";

    std::cout << "Available seats: "
              << availableSeats
              << "\n";
}