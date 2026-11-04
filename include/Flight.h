#ifndef FLIGHT_H
#define FLIGHT_H

#include <string>

class Flight {

private:
    std::string flightNumber;
    std::string origin;
    std::string destination;

    int totalSeats;
    int availableSeats;

public:
    Flight(
        const std::string& flightNumber,
        const std::string& origin,
        const std::string& destination,
        int totalSeats
    );

    // Getters
    std::string getFlightNumber() const;
    std::string getOrigin() const;
    std::string getDestination() const;

    int getTotalSeats() const;
    int getAvailableSeats() const;

    // Seat management
    bool reserveSeat();
    bool releaseSeat();

    // Display information
    void displayInfo() const;
};

#endif